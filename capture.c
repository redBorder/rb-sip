// Copyright (C) 2023 Eneo Tecnologia S.L.
// Miguel Álvarez Adsuara <malvarez@redborder.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <stdio.h>
#include <stdlib.h>
#include <pcap.h>
#include <arpa/inet.h>
#include <netinet/ether.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <ctype.h>
#include <stdbool.h>
#include "call.h"
#include "config.h"
#include "sox.h"
#include "kafka.h"
#include "binary.h"

/*
    Define config values
*/

#define KAFKA_BROKERS "10.0.209.10:9092"
#define KAFKA_TOPIC "rb_malware"
#define KAFKA_INIT_PRODUCER() kafka_init_producer(KAFKA_BROKERS)
#define MAX_JSON_BUFFER_SIZE 1024

ConfigEntry* config_entries;

/*
    Global
*/

rd_kafka_t *rk;
char *topic = KAFKA_TOPIC;
struct CallHistory callHistory;
int rtpsecuence = 0;

/*
    Main program below
*/

char* create_json_string(const struct CallState *call_state) {
    char json_buffer[MAX_JSON_BUFFER_SIZE];

    snprintf(json_buffer, MAX_JSON_BUFFER_SIZE,
             "{\"encoding\":\"%s\",\"status\":\"%s\",\"call_id\":\"%s\",\"call_time\":%ld,\"source_ip\":\"%s\",\"destination_ip\":\"%s\",\"source_port\":%u,\"destination_port\":%u,\"incoming_filename\":\"%s\",\"outgoing_filename\":\"%s\"}",
             call_state->encoding, call_state->status, call_state->call_id, (long)call_state->call_time,
             call_state->source_ip, call_state->destination_ip,
             call_state->source_port, call_state->destination_port,
             call_state->incoming_filename, call_state->outgoing_filename);

    return strdup(json_buffer);
}

void write_payload_to_file(const char* filename, const unsigned char* payload, size_t payload_length) {
    FILE* file = fopen(filename, "ab+");
    if (file) {
        fwrite(payload, 1, payload_length, file);
        fclose(file);
        printf("Payload written to file: %s\n", filename);
    } else {
        printf("Error opening file: %s\n", filename);
    }
}

bool file_exists(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file) {
        fclose(file);
        return true;
    }
    return false;
}

char* extract_call_id(const unsigned char* payload) {
    char call_id[] = "Call-ID:";
    char* call_id_ptr = strstr((char*)payload, call_id);

    if (call_id_ptr) {
        call_id_ptr += sizeof(call_id) - 1;
        while (*call_id_ptr == ' ') {
            call_id_ptr++;
        }
        static char call_id_value[256];
        int i = 0;
        while (*call_id_ptr && *call_id_ptr != '\r' && *call_id_ptr != '\n') {
            call_id_value[i++] = *call_id_ptr++;
        }
        call_id_value[i] = '\0';

        return call_id_value;
    } else {
        return NULL;
    }
}

int get_call_index_by_ip(const char* ip_address) {
    int i = 0;
    for (i; i < callHistory.num_calls; i++) {
        if (strcmp(callHistory.calls[i].source_ip, ip_address) == 0 ||
            strcmp(callHistory.calls[i].destination_ip, ip_address) == 0) {
            return i;
        }
    }
    return -1;
}

void packet_handler(unsigned char *user_data, const struct pcap_pkthdr *pkthdr, const unsigned char *packet) {
    struct ether_header *eth_header = (struct ether_header *)packet;
    uint16_t ether_type = ntohs(eth_header->ether_type);

    if (ether_type != ETHERTYPE_IP) {
        printf("Not an IP packet. Skipping...\n");
        return;
    }

    struct ip *ip_header = (struct ip *)(packet + sizeof(struct ether_header));
    int ip_header_length = ip_header->ip_hl * 4;

    if (ip_header->ip_p != IPPROTO_UDP) {
        printf("Not a UDP packet. Skipping...\n");
        return;
    }

    char ip_src[INET_ADDRSTRLEN];
    char ip_dst[INET_ADDRSTRLEN];

    if (inet_ntop(AF_INET, &(ip_header->ip_src), ip_src, INET_ADDRSTRLEN) == NULL) {
        perror("Error converting source IP address");
        return;
    }

    if (inet_ntop(AF_INET, &(ip_header->ip_dst), ip_dst, INET_ADDRSTRLEN) == NULL) {
        perror("Error converting destination IP address");
        return;
    }

    struct udphdr *udp_header = (struct udphdr *)((unsigned char *)ip_header + ip_header_length);
    unsigned char *payload = (unsigned char *)((unsigned char *)udp_header + sizeof(struct udphdr));
    int payload_length = ntohs(ip_header->ip_len) - ip_header_length - sizeof(struct udphdr);
    size_t offset = 0;
    if (isupper(readUint8(payload, offset))) {
        if (payload_length >= 6 && strncasecmp((char*)payload, "INVITE", 6) == 0) {
            char* call_id_value = extract_call_id(payload);
            struct CallState new_call;
            set_call_state(&new_call, "STARTED", ip_src, ip_dst, ntohs(udp_header->source), ntohs(udp_header->dest));
            strcpy(new_call.call_id, call_id_value);
            addCall(&callHistory, &new_call);
            if (file_exists(new_call.outgoing_filename)) {
                remove(new_call.outgoing_filename);
            }
            if(file_exists(new_call.incoming_filename)){
                remove(new_call.incoming_filename);
            }
        } else if (payload_length >= 3 && strncasecmp((char*)payload, "BYE", 3) == 0  || payload_length >= 6 && strncasecmp((char*)payload, "CANCEL", 6) == 0) {
            char* call_id_value = extract_call_id(payload);
            int call_index = -1;
            int call_history_length = 0; 
            for (call_history_length; call_history_length < callHistory.num_calls; call_history_length++) {
                if (strcmp(callHistory.calls[call_history_length].call_id, call_id_value) == 0) {
                    call_index = call_history_length;
                    break;
                }
            }
            if (call_index != -1 && strcmp(callHistory.calls[call_index].status, "ENDED")) {
                sprintf(callHistory.calls[call_index].status, "ENDED");
                char incoming_wav_filename[256];
                char outgoing_wav_filename[256];

                strcpy(incoming_wav_filename, callHistory.calls[call_index].incoming_filename);
                strcat(incoming_wav_filename, ".wav");

                strcpy(outgoing_wav_filename, callHistory.calls[call_index].outgoing_filename);
                strcat(outgoing_wav_filename, ".wav");

                sox_decode_audio(callHistory.calls[call_index].incoming_filename, incoming_wav_filename, 8);
                sox_decode_audio(callHistory.calls[call_index].outgoing_filename, outgoing_wav_filename, 8);
                printf("Audio decoded using sox.");
                char *json_string = create_json_string(&callHistory.calls[call_index]);
                if (kafka_produce_message(rk, topic, json_string) != 0) {
                    fprintf(stderr, "Failed to produce Kafka message.\n");
                    kafka_cleanup(rk);
                    return 1;
                }

            }
        }
    }
    uint8_t OpCode = readUint8(payload, offset);
    offset += 1;
    if ((OpCode == 0x80 || OpCode == 0x90) && payload_length >= 12) {
        int call_index = get_call_index_by_ip(inet_ntoa(ip_header->ip_src));
        if (call_index == -1) {
            call_index = get_call_index_by_ip(inet_ntoa(ip_header->ip_dst));
        }
        if (call_index != -1) {
            uint8_t payload_type = readUint8(payload, offset) & 0x7F;
            offset += 1;
            sprintf(callHistory.calls[call_index].encoding, get_raw_encoding(payload_type));
            uint16_t sequence_number = readUint16BE(payload, offset);
            offset += 2;
            uint32_t timestamp = readUint32BE(payload, offset);
            offset += 4;
            uint32_t ssrc = readUint32BE(payload, offset);
            offset += 4;
            struct CallState* current_call = NULL;
            current_call = &callHistory.calls[call_index];
            const unsigned char* payload_to_write = payload + 12;
            size_t payload_length_to_write = payload_length - 12;

            if (payload_length_to_write > 0) {
                if (strcmp(inet_ntoa(ip_header->ip_src), current_call->destination_ip) == 0 && rtpsecuence != sequence_number) {
                    write_payload_to_file(current_call->outgoing_filename, payload_to_write, payload_length_to_write);
                } else if (rtpsecuence != sequence_number) {
                    write_payload_to_file(current_call->incoming_filename, payload_to_write, payload_length_to_write);
                }
            }
            rtpsecuence = sequence_number;
        }
    }
}

void read_config_file(char* config){
    size_t num_entries = 0;
    ConfigEntry* config_entries = parse_config_file(config, &num_entries);
    if (config_entries) {
        size_t i = 0;
        for (i; i < num_entries; i++) {
            printf("%s %s\n", config_entries[i].key, config_entries[i].value);
        }
        free(config_entries);
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <interface> <config>\n", argv[0]);
        return 1;
    }
    read_config_file(argv[2]);
    rk = KAFKA_INIT_PRODUCER();
    char interface[PCAP_ERRBUF_SIZE];
    pcap_t *handle;
    const char *dev = argv[1];

    handle = pcap_open_live(dev, BUFSIZ, 1, 1000, interface);

    if (handle == NULL) {
        fprintf(stderr, "Error opening device %s: %s\n", dev, interface);
        return 1;
    }

    initCallHistory(&callHistory, 10);

    struct bpf_program fp;
    char filter_exp[] = "udp";

    if (pcap_compile(handle, &fp, filter_exp, 0, PCAP_NETMASK_UNKNOWN) == -1) {
        fprintf(stderr, "Error compiling filter: %s\n", pcap_geterr(handle));
        pcap_close(handle);
        return 1;
    }

    if (pcap_setfilter(handle, &fp) == -1) {
        fprintf(stderr, "Error setting filter: %s\n", pcap_geterr(handle));
        pcap_freecode(&fp);
        pcap_close(handle);
        return 1;
    }

    pcap_loop(handle, 0, packet_handler, NULL);
    pcap_freecode(&fp);
    pcap_close(handle);
    return 0;
}
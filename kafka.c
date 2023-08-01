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

#include "kafka.h"

rd_kafka_t* kafka_init_producer(const char *brokers) {
    rd_kafka_t *rk;
    rd_kafka_conf_t *conf;

    conf = rd_kafka_conf_new();

    if (rd_kafka_conf_set(conf, "bootstrap.servers", brokers, NULL, 0) != RD_KAFKA_CONF_OK) {
        fprintf(stderr, "Failed to set broker list: %s\n", rd_kafka_err2str(rd_kafka_last_error()));
        return NULL;
    }

    rk = rd_kafka_new(RD_KAFKA_PRODUCER, conf, NULL, 0);
    if (!rk) {
        fprintf(stderr, "Failed to create Kafka producer: %s\n", rd_kafka_err2str(rd_kafka_last_error()));
        return NULL;
    }

    return rk;
}

int kafka_produce_message(rd_kafka_t *rk, const char *topic, const char *message) {
    rd_kafka_topic_t *rkt;
    rd_kafka_conf_t *conf;

    conf = rd_kafka_topic_conf_new();

    rkt = rd_kafka_topic_new(rk, topic, conf);
    if (!rkt) {
        fprintf(stderr, "Failed to create Kafka topic: %s\n", rd_kafka_err2str(rd_kafka_last_error()));
        return -1;
    }

    rd_kafka_resp_err_t err = rd_kafka_produce(
        rkt,
        RD_KAFKA_PARTITION_UA,
        RD_KAFKA_MSG_F_COPY,
        (void *)message,
        strlen(message),
        NULL, 0, NULL);

    if (err) {
        fprintf(stderr, "Failed to produce Kafka message: %s\n", rd_kafka_err2str(err));
        return -1;
    }

    rd_kafka_poll(rk, 0);
    rd_kafka_topic_destroy(rkt);

    return 0; 
}

void kafka_cleanup(rd_kafka_t *rk) {
    rd_kafka_destroy(rk);
}
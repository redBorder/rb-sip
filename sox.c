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

#include "sox.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* get_raw_encoding(int byte) {
    int pt = byte & 0x7F;

    switch (pt) {
        case 0:
            return "PCMU (G.711 mu-law)";
        case 8:
            return "PCMA (G.711 A-law)";
        default:
            return "Unknown or unsupported encoding";
    }
}

const char* get_sox_encoding(int byte) {
    int pt = byte & 0x7F;

    switch (pt) {
        case 0:
            return "mu-law";
        case 8:
            return "a-law";
        default:
            return "Unknown or unsupported encoding";
    }
}

int sox_decode_audio(const char* input_file, const char* output_file, int payload_type) {
    if (input_file == NULL || output_file == NULL) {
        printf("Error: Invalid file names.\n");
        return -1;
    }

    const char* encoding = get_sox_encoding(payload_type);
    printf("Decoding RTP audio using sox input = %s, output = %s, payload = %d\n", input_file, output_file, payload_type);

    char command[256];
    snprintf(command, sizeof(command), "sox -t raw -r 8000 -c 1 -e %s %s %s", encoding, input_file, output_file);

    int result = system(command);
    return result;
}
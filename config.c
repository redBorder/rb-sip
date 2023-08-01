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
#include <string.h>

#define MAX_LINE_LENGTH 1024

typedef struct {
    char key[MAX_LINE_LENGTH];
    char value[MAX_LINE_LENGTH];
} ConfigEntry;

ConfigEntry* parse_config_file(const char* filename, size_t* num_entries) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file: %s\n", filename);
        return NULL;
    }

    ConfigEntry* config_entries = NULL;
    size_t num_entries_allocated = 0;
    size_t current_entry = 0;
    char line[MAX_LINE_LENGTH];

    while (fgets(line, MAX_LINE_LENGTH, file)) {
        char* key = strtok(line, " \t\n=");
        char* value = strtok(NULL, "\n");

        if (key && value) {
            if (current_entry >= num_entries_allocated) {
                num_entries_allocated += 10;
                config_entries = (ConfigEntry*)realloc(config_entries, num_entries_allocated * sizeof(ConfigEntry));
            }

            strcpy(config_entries[current_entry].key, key);
            strcpy(config_entries[current_entry].value, value);

            current_entry++;
        }
    }

    fclose(file);
    *num_entries = current_entry;
    return config_entries;
}
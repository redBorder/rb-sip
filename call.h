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

#ifndef CALL_STATE_H
#define CALL_STATE_H

#include <time.h>

struct CallState {
    char encoding[256];
    char status[20];
    char call_id[256];
    time_t call_time;
    char source_ip[16];
    char destination_ip[16];
    unsigned int source_port;
    unsigned int destination_port;
    char incoming_filename[256];
    char outgoing_filename[256];
};

struct CallHistory {
    struct CallState *calls;
    size_t num_calls;
    size_t capacity;
};


void set_call_state(struct CallState *call_state, const char *status,
                    const char *source_ip, const char *destination_ip,
                    unsigned int source_port, unsigned int destination_port);

void initCallHistory(struct CallHistory *callHistory, size_t initialCapacity);

void resizeCallHistory(struct CallHistory *callHistory, size_t newCapacity);

void addCall(struct CallHistory *callHistory, const struct CallState *call);

void freeCallHistory(struct CallHistory *callHistory);
#endif
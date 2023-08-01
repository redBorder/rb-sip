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

#include "call.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

void set_call_state(struct CallState *call_state, const char *status,
                    const char *source_ip, const char *destination_ip,
                    unsigned int source_port, unsigned int destination_port) {
    sprintf(call_state->status, "%s", status);
    time(&call_state->call_time);
    sprintf(call_state->source_ip, "%s", source_ip);
    sprintf(call_state->destination_ip, "%s", destination_ip);
    call_state->source_port = source_port;
    call_state->destination_port = destination_port;
    snprintf(call_state->incoming_filename, sizeof(call_state->incoming_filename), "/tmp/incoming_rtp_%s.raw", call_state->source_ip);
    snprintf(call_state->outgoing_filename, sizeof(call_state->outgoing_filename), "/tmp/outgoing_rtp_%s.raw", call_state->destination_ip);
}

void initCallHistory(struct CallHistory *callHistory, size_t initialCapacity) {
    callHistory->calls = (struct CallState *)malloc(initialCapacity * sizeof(struct CallState));
    callHistory->num_calls = 0;
    callHistory->capacity = initialCapacity;
}

void resizeCallHistory(struct CallHistory *callHistory, size_t newCapacity) {
    callHistory->calls = (struct CallState *)realloc(callHistory->calls, newCapacity * sizeof(struct CallState));
    callHistory->capacity = newCapacity;
}

void addCall(struct CallHistory *callHistory, const struct CallState *call) {
    if (callHistory->num_calls == callHistory->capacity) {
        resizeCallHistory(callHistory, callHistory->capacity * 2);
    }
    callHistory->calls[callHistory->num_calls++] = *call;
}

void freeCallHistory(struct CallHistory *callHistory) {
    free(callHistory->calls);
    callHistory->num_calls = 0;
    callHistory->capacity = 0;
}

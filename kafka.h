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

#ifndef KAFKA_H
#define KAFKA_H

#include <librdkafka/rdkafka.h>

rd_kafka_t* kafka_init_producer(const char *brokers);

int kafka_produce_message(rd_kafka_t *rk, const char *topic, const char *message);

void kafka_cleanup(rd_kafka_t *rk);

#endif  // KAFKA_H
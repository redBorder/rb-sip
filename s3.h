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

#ifndef S3_H
#define S3_H

#include <stdbool.h>

bool s3_init();

void s3_cleanup();

bool s3_upload_file(const char *bucket_name, const char *object_key, const char *file_path, const char *s3_host);

#endif  // S3_H
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

#include "s3.h"
#include <aws/common/common.h>
#include <aws/core/Aws.h>
#include <aws/s3/s3_client.h>
#include <aws/s3/s3_client_config.h>
#include <aws/s3/model/PutObjectRequest.h>
#include <aws/s3/model/GetObjectRequest.h>
#include <stdio.h>

bool s3_init() {
    if (aws_common_library_init(aws_default_allocator()) == AWS_ERROR_SUCCESS) {
        return true;
    }
    return false;
}

void s3_cleanup() {
    aws_common_library_clean_up();
}

bool s3_upload_file(const char *bucket_name, const char *object_key, const char *file_path, const char *s3_host) {
    struct aws_s3_client_config s3_config = {0};
    s3_config.bucket_name = aws_byte_cursor_from_c_str(bucket_name);
    s3_config.endpoint = aws_byte_cursor_from_c_str(s3_host);

    struct aws_s3_client *s3_client = aws_s3_client_new(&s3_config);

    aws_s3_put_object_request_destroy(put_request);
    free(buffer);
    aws_s3_client_destroy(s3_client);
    return true;
}

// Copyright 2018 JDCLOUD.COM
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// NOTE: This file is modified from AWS V4 Signer algorithm.

#include "jdcloud_signer/http/HttpTypes.h"

namespace HttpMethodMapper
{
    const char *GetNameForHttpMethod(HttpMethod httpMethod) {
        switch (httpMethod) {
            case HttpMethod::HTTP_GET:
                return "GET";
            case HttpMethod::HTTP_POST:
                return "POST";
            case HttpMethod::HTTP_DELETE:
                return "DELETE";
            case HttpMethod::HTTP_PUT:
                return "PUT";
            case HttpMethod::HTTP_HEAD:
                return "HEAD";
            case HttpMethod::HTTP_PATCH:
                return "PATCH";
            default:
                return "GET";
        }
    }
}

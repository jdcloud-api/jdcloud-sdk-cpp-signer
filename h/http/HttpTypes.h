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

#pragma once

#include <string>
#include <map>

using std::pair;
using std::map;
using std::string;

/**
 * Models Http methods.
 */
enum class HttpMethod
{
    HTTP_GET,
    HTTP_POST,
    HTTP_DELETE,
    HTTP_PUT,
    HTTP_HEAD,
    HTTP_PATCH
};

namespace HttpMethodMapper
{
    /**
     * Gets the string value of an httpMethod.
     */
    const char *GetNameForHttpMethod(HttpMethod httpMethod);
}

typedef pair<string, string> HeaderValuePair;
typedef map<string, string> HeaderValueCollection;

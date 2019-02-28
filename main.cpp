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

#include <iostream>
#include <cassert>
#include "jdcloud_signer/Credential.h"
#include "jdcloud_signer/JdcloudSigner.h"
#include "jdcloud_signer/http/HttpTypes.h"
#include "jdcloud_signer/http/HttpRequest.h"
#include "jdcloud_signer/logging/Logging.h"
#include "jdcloud_signer/logging/ConsoleLogSystem.h"

using namespace std;
using namespace jdcloud_signer;

void TestRequestWithoutBody() {
    ConsoleLogSystem* cls = new ConsoleLogSystem(LogLevel::Debug);
    shared_ptr<ConsoleLogSystem> log(cls);
    InitializeLogging(log);

    HttpRequest request(URI("http://www.jdcloud-api.com/v1/regions/cn-north-1/instances?pageNumber=2&pageSize=10"), HttpMethod::HTTP_GET);
    request.SetHeaderValue(CONTENT_TYPE_HEADER, "application/json");
    request.SetHeaderValue(USER_AGENT_HEADER, "JdcloudSdkGo/1.0.2 vm/0.7.4");

    Credential credential("AK", "SK");
    JdcloudSigner signer(credential, "vm", "cn-north-1");

    bool result = signer.SignRequest(request);

    // put header "Authorization" to your own request header

    assert(result);
}

void TestRequestWithBody() {
    ConsoleLogSystem* cls = new ConsoleLogSystem(LogLevel::Debug);
    shared_ptr<ConsoleLogSystem> log(cls);
    InitializeLogging(log);

    HttpRequest request(URI("http://www.jdcloud-api.com/v1/regions/cn-north-1/instances"), HttpMethod::HTTP_POST);
    request.SetHeaderValue(CONTENT_TYPE_HEADER, "application/json");
    request.SetHeaderValue(USER_AGENT_HEADER, "JdcloudSdkGo/1.0.2 vm/0.7.4");

    stringstream* content = new stringstream("test body");
    shared_ptr<iostream> body(content);
    request.AddContentBody(body);

    Credential credential("AK", "SK");
    JdcloudSigner signer(credential, "vm", "cn-north-1");

    bool result = signer.SignRequest(request);

    // put header "Authorization" to your own request header

    assert(result);
}

int main() {
    TestRequestWithoutBody();
    TestRequestWithBody();

    return 0;
}

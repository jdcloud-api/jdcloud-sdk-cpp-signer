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
#include <set>
#include <iostream>
#include <sstream>
#include <algorithm>
#include "jdcloud_signer/Credential.h"
#include "jdcloud_signer/util/crypto/Sha256.h"
#include "jdcloud_signer/util/crypto/Sha256HMAC.h"
#include "jdcloud_signer/util/DateTime.h"
#include "jdcloud_signer/http/HttpRequest.h"

using std::string;
using std::map;
using std::set;
using std::iostream;
using std::stringstream;
using std::unique_ptr;


class JdcloudSigner
{
public:
    JdcloudSigner(const Credential& credential, const string& serviceName, const string& region);

    virtual ~JdcloudSigner();

    bool SignRequest(HttpRequest& request) const;

private:
    bool ShouldSignHeader(const string& header) const;
    string GenerateSignature(const Credential& credentials, const string& stringToSign, const string& simpleDate) const;
    string GenerateSignature(const string& stringToSign, const string& key) const;
    string GenerateStringToSign(const string& dateValue, const string& simpleDate, const string& canonicalRequestHash,
                                const string& region, const string& serviceName) const;

    string ComputeHash(const string& secretKey, const string& simpleDate, const string& region,
                        const string& serviceName) const;
    string ComputePayloadHash(HttpRequest& request) const;
    DateTime GetSigningTimestamp() const { return DateTime::Now(); }

    Credential m_credential;
    string m_serviceName;
    string m_region;
    set<string> m_unsignedHeaders;
    unique_ptr<Sha256> m_hash;
    unique_ptr<Sha256HMAC> m_hmac;
};

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

namespace jdcloud_signer {

class JdcloudSigner
{
public:
    JdcloudSigner(const Credential& credential, const std::string& serviceName, const std::string& region);

    virtual ~JdcloudSigner();

    bool SignRequest(HttpRequest& request) const;

private:
    bool ShouldSignHeader(const std::string& header) const;
    std::string GenerateSignature(const Credential& credentials, const std::string& stringToSign, const std::string& simpleDate) const;
    std::string GenerateSignature(const std::string& stringToSign, const std::string& key) const;
    std::string GenerateStringToSign(const std::string& dateValue, const std::string& simpleDate, const std::string& canonicalRequestHash,
                                const std::string& region, const std::string& serviceName) const;

    std::string ComputeHash(const std::string& secretKey, const std::string& simpleDate, const std::string& region,
                        const std::string& serviceName) const;
    std::string ComputePayloadHash(HttpRequest& request) const;
    DateTime GetSigningTimestamp() const { return DateTime::Now(); }

    Credential m_credential;
    std::string m_serviceName;
    std::string m_region;
    std::set<std::string> m_unsignedHeaders;
    std::unique_ptr<Sha256> m_hash;
    std::unique_ptr<Sha256HMAC> m_hmac;
};

}

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

#include "jdcloud_signer/util/crypto/Sha256HMAC.h"

#include <openssl/hmac.h>
#include "jdcloud_signer/util/crypto/HashingUtils.h"

using namespace std;

class HMACRAIIGuard {
public:
    HMACRAIIGuard()
    {
        m_ctx = new HMAC_CTX;
        assert(m_ctx != nullptr);
    }

    ~HMACRAIIGuard()
    {
        delete m_ctx;
        m_ctx = nullptr;
    }

    HMAC_CTX* getResource() {
        return m_ctx;
    }
private:
    HMAC_CTX *m_ctx;
};

HashResult Sha256HMAC::Calculate(const string& toSign, const string& secret)
{
    unsigned int length = 64;
    auto digest = (unsigned char*)malloc(length);
    memset(digest, 0, length);

    HMACRAIIGuard guard;
    HMAC_CTX* m_ctx = guard.getResource();
    HMAC_CTX_init(m_ctx);

    HMAC_Init_ex(m_ctx, secret.c_str(), static_cast<int>(secret.size()), EVP_sha256(), NULL);
    HMAC_Update(m_ctx, (unsigned char*)toSign.c_str(), toSign.size());
    HMAC_Final(m_ctx, digest, &length);

    HMAC_CTX_cleanup(m_ctx);

    string result((const char*) digest, length);
    free(digest);

    return HashResult(result);
}

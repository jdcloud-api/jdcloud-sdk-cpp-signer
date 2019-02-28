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

#include "jdcloud_signer/util/crypto/Sha256.h"
#include <openssl/sha.h>
#include <openssl/evp.h>
#include "jdcloud_signer/util/crypto/HashingUtils.h"
#include "jdcloud_signer/util/logging/LogMacros.h"

namespace jdcloud_signer {

class OpensslCtxRAIIGuard
{
public:
    OpensslCtxRAIIGuard()
    {
        m_ctx = EVP_MD_CTX_create();
        assert(m_ctx != nullptr);
    }

    ~OpensslCtxRAIIGuard()
    {
        EVP_MD_CTX_destroy(m_ctx);
        m_ctx = nullptr;
    }

    EVP_MD_CTX* getResource()
    {
        return m_ctx;
    }
private:
    EVP_MD_CTX *m_ctx;
};

HashResult Sha256::Calculate(const std::string& str)
{
    OpensslCtxRAIIGuard guard;
    auto ctx = guard.getResource();
    EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr);
    EVP_DigestUpdate(ctx, str.c_str(), str.size());

    size_t length = (size_t)EVP_MD_size(EVP_sha256());
    auto hash = (unsigned char*)malloc(length);
    memset(hash, 0, length);
    EVP_DigestFinal(ctx, hash, nullptr);

    std::string result = HashingUtils::HexEncode(hash, length);
    free(hash);
    return HashResult(result);
}

HashResult Sha256::Calculate(std::istream& stream)
{
    OpensslCtxRAIIGuard guard;
    auto ctx = guard.getResource();

    EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr);

    auto currentPos = stream.tellg();
    if ((int)currentPos == -1)
    {
        currentPos = 0;
        stream.clear();
    }

    stream.seekg(0, stream.beg);

    const int bufLength = 4096;
    char streamBuffer[bufLength];
    while (stream.good())
    {
        stream.read(streamBuffer, bufLength);
        auto bytesRead = stream.gcount();

        if (bytesRead > 0)
        {
            EVP_DigestUpdate(ctx, streamBuffer, static_cast<size_t>(bytesRead));
        }
    }

    stream.clear();
    stream.seekg(currentPos, stream.beg);

    size_t length = (size_t)EVP_MD_size(EVP_sha256());
    auto hash = (unsigned char*)malloc(length);
    memset(hash, 0, length);
    EVP_DigestFinal(ctx, hash, nullptr);

    std::string result = HashingUtils::HexEncode(hash, length);
    free(hash);
    return HashResult(result);
}

}

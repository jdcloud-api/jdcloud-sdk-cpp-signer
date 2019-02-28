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

#include "jdcloud_signer/JdcloudSignerImpl.h"

#ifdef WIN32
#include <objbase.h>
#else
#include <uuid/uuid.h>
#endif
#include "jdcloud_signer/util/crypto/HashingUtils.h"
#include "jdcloud_signer/util/StringUtils.h"
#include "jdcloud_signer/http/HttpTypes.h"
#include "jdcloud_signer/logging/LogMacros.h"

using namespace std;

namespace jdcloud_signer {

static const char* EQ = "=";
static const char* SIGNATURE = "Signature";
static const char* HMAC_SHA256 = "JDCLOUD2-HMAC-SHA256";
static const char* JDCLOUD_REQUEST = "jdcloud2_request";
static const char* SIGNED_HEADERS = "SignedHeaders";
static const char* CREDENTIAL = "Credential";
static const char* NEWLINE = "\n";
static const char* UNSIGNED_PAYLOAD = "UNSIGNED-PAYLOAD";
static const char* SIGNING_KEY = "JDCLOUD2";
static const char* LONG_DATE_FORMAT_STR = "%Y%m%dT%H%M%SZ";
static const char* SIMPLE_DATE_FORMAT_STR = "%Y%m%d";
static const char* EMPTY_STRING_SHA256 = "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";
static const char* logTag = "JdcloudAuthSigner";

JdcloudSignerImpl::JdcloudSignerImpl(const Credential& credential, const string& serviceName, const string& region) :
    m_credential(credential),
    m_serviceName(serviceName),
    m_region(region),
    m_unsignedHeaders({USER_AGENT_HEADER, AUTHORIZATION_HEADER}),
    m_hash(unique_ptr<Sha256>(new Sha256)),
    m_hmac(unique_ptr<Sha256HMAC>(new Sha256HMAC))
{
}

JdcloudSignerImpl::~JdcloudSignerImpl()
{
}

static HeaderValueCollection CanonicalizeHeaders(HeaderValueCollection&& headers)
{
    HeaderValueCollection canonicalHeaders;
    for (const auto& header : headers)
    {
        auto trimmedHeaderName = StringUtils::Trim(header.first.c_str());
        auto trimmedHeaderValue = StringUtils::Trim(header.second.c_str());

        //multiline gets converted to line1,line2,etc...
        auto headerMultiLine = StringUtils::SplitOnLine(trimmedHeaderValue);
        string headerValue = headerMultiLine.size() == 0 ? "" : headerMultiLine[0];

        if (headerMultiLine.size() > 1)
        {
            for(size_t i = 1; i < headerMultiLine.size(); ++i)
            {
                headerValue += ",";
                headerValue += StringUtils::Trim(headerMultiLine[i].c_str());
            }
        }

        //duplicate spaces need to be converted to one.
        string::iterator new_end =
                std::unique(headerValue.begin(), headerValue.end(),
                            [=](char lhs, char rhs) { return (lhs == rhs) && (lhs == ' '); }
                );
        headerValue.erase(new_end, headerValue.end());

        canonicalHeaders[trimmedHeaderName] = headerValue;
    }

    return canonicalHeaders;
}

static string CanonicalizeRequestSigningString(HttpRequest& request, bool urlEscapePath)
{
    request.CanonicalizeRequest();
    stringstream signingStringStream;
    signingStringStream << HttpMethodMapper::GetNameForHttpMethod(request.GetMethod());

    URI uriCpy = request.GetUri();
    // Many services do not decode the URL before calculating SignatureV4 on their end.
    // This results in the signature getting calculated with a double encoded URL.
    // That means we have to double encode it here for the signature to match on the service side.
    if(urlEscapePath)
    {
        // RFC3986 is how we encode the URL before sending it on the wire.
        auto rfc3986EncodedPath = URI::URLEncodePathRFC3986(uriCpy.GetPath());
        uriCpy.SetPath(rfc3986EncodedPath);
        // However, SignatureV4 uses this URL encoding scheme
        signingStringStream << NEWLINE << uriCpy.GetURLEncodedPath() << NEWLINE;
    }
    else
    {
        // For the services that DO decode the URL first; we don't need to double encode it.
        uriCpy.SetPath(uriCpy.GetURLEncodedPath());
        signingStringStream << NEWLINE << uriCpy.GetPath() << NEWLINE;
    }

    if (request.GetQueryString().size() > 1 && request.GetQueryString().find("=") != std::string::npos)
    {
        signingStringStream << request.GetQueryString().substr(1) << NEWLINE;
    }
    else if (request.GetQueryString().size() > 1)
    {
        signingStringStream << request.GetQueryString().substr(1) << "=" << NEWLINE;
    }
    else
    {
        signingStringStream << NEWLINE;
    }

    return signingStringStream.str();
}

#ifdef WIN32
static string GetUUID()
{
    GUID guid;
    char buf[64] = { 0 };
    CoCreateGuid(&guid);
    _snprintf_s(buf,
        sizeof(buf),
        "{%08X%04X%04X%2X%02X%02X%02X%02X%02X%02X%02X}",
        guid.Data1, guid.Data2, guid.Data3,
        guid.Data4[0], guid.Data4[1],
        guid.Data4[2], guid.Data4[3],
        guid.Data4[4], guid.Data4[5],
        guid.Data4[6], guid.Data4[7]);
    return std::string(buf);
}
#else
static string GetUUID()
{
    uuid_t uuid;
    //The UUID is 16 bytes (128 bits) long
    uuid_generate(uuid);

    return HashingUtils::HexEncode((unsigned char *)uuid, 16);
}
#endif

bool JdcloudSignerImpl::SignRequest(HttpRequest& request) const
{
    //don't sign anonymous requests
    if (m_credential.GetAccessKey().empty() || m_credential.GetSecretKey().empty())
    {
        return false;
    }

    string payloadHash(UNSIGNED_PAYLOAD);
    payloadHash.assign(ComputePayloadHash(request));
    if (payloadHash.empty())
    {
        return false;
    }

    //calculate date header to use in internal signature (this also goes into date header).
    DateTime now = GetSigningTimestamp();
    string dateHeaderValue = now.ToGmtString(LONG_DATE_FORMAT_STR);
    request.SetHeaderValue(DATE_HEADER, dateHeaderValue);
    request.SetHeaderValue(NONCE_HEADER, GetUUID());

    std::stringstream headersStream;
    std::stringstream signedHeadersStream;

    for (const auto& header : CanonicalizeHeaders(request.GetHeaders()))
    {
        if(ShouldSignHeader(header.first))
        {
            headersStream << header.first.c_str() << ":" << header.second.c_str() << NEWLINE;
            signedHeadersStream << header.first.c_str() << ";";
        }
    }

    string canonicalHeadersString = headersStream.str();
    LOGSTREAM_DEBUG(logTag, "Canonical Header String: \n" << canonicalHeadersString);

    //calculate signed headers parameter
    string signedHeadersValue = signedHeadersStream.str();
    //remove that last semi-colon
    if (!signedHeadersValue.empty())
    {
        signedHeadersValue.pop_back();
    }

    LOGSTREAM_DEBUG(logTag, "Signed Headers value:" << signedHeadersValue);

    //generate generalized canonicalized request string.
    string canonicalRequestString = CanonicalizeRequestSigningString(request, false);

    //append v4 stuff to the canonical request string.
    canonicalRequestString.append(canonicalHeadersString);
    canonicalRequestString.append(NEWLINE);
    canonicalRequestString.append(signedHeadersValue);
    canonicalRequestString.append(NEWLINE);
    canonicalRequestString.append(payloadHash);

    LOGSTREAM_DEBUG(logTag, "Canonical Request String: \n" << canonicalRequestString);

    //now compute sha256 on that request string
    auto hashResult = m_hash->Calculate(canonicalRequestString);
    if (!hashResult.IsSuccess())
    {
        LOGSTREAM_ERROR(logTag, "Failed to hash (sha256) request string");
        LOGSTREAM_DEBUG(logTag, "The request string is: \"" << canonicalRequestString << "\"");
        return false;
    }

    string cannonicalRequestHash = hashResult.GetResult();
    string simpleDate = now.ToGmtString(SIMPLE_DATE_FORMAT_STR);

    string stringToSign = GenerateStringToSign(dateHeaderValue, simpleDate, cannonicalRequestHash, m_region,
                                                    m_serviceName);
    auto finalSignature = GenerateSignature(m_credential, stringToSign, simpleDate);

    stringstream ss;
    ss << HMAC_SHA256 << " " << CREDENTIAL << EQ << m_credential.GetAccessKey() << "/" << simpleDate
        << "/" << m_region << "/" << m_serviceName << "/" << JDCLOUD_REQUEST << ", " << SIGNED_HEADERS << EQ
        << signedHeadersValue << ", " << SIGNATURE << EQ << finalSignature;

    auto authString = ss.str();
    LOGSTREAM_DEBUG(logTag, "Signing request with: " << authString);
    request.SetAuthorization(authString);

    return true;
}

bool JdcloudSignerImpl::ShouldSignHeader(const string& header) const
{
    return m_unsignedHeaders.find(header.c_str()) == m_unsignedHeaders.cend();
}

string JdcloudSignerImpl::ComputePayloadHash(HttpRequest& request) const
{
    if (!request.GetContentBody())
    {
        LOGSTREAM_DEBUG(logTag, "Using cached empty string sha256 " << EMPTY_STRING_SHA256 << " because payload is empty.");
        return EMPTY_STRING_SHA256;
    }

    //compute hash on payload if it exists.
    auto hashResult = m_hash->Calculate(*request.GetContentBody());

    if(request.GetContentBody())
    {
        request.GetContentBody()->clear();
        request.GetContentBody()->seekg(0);
    }

    if (!hashResult.IsSuccess())
    {
        LOGSTREAM_ERROR(logTag, "Unable to hash (sha256) request body");
        return "";
    }

    auto sha256Digest = hashResult.GetResult();

    string payloadHash(sha256Digest);
    LOGSTREAM_DEBUG(logTag, "Calculated sha256 " << payloadHash << " for payload.");
    return payloadHash;
}


string JdcloudSignerImpl::GenerateStringToSign(const string& dateValue, const string& simpleDate,
                                            const string& canonicalRequestHash, const string& region, const string& serviceName) const
{
    //generate the actual string we will use in signing the final request.
    stringstream ss;

    ss << HMAC_SHA256 << NEWLINE << dateValue << NEWLINE << simpleDate << "/" << region << "/"
        << serviceName << "/" << JDCLOUD_REQUEST << NEWLINE << canonicalRequestHash;

    return ss.str();
}

string JdcloudSignerImpl::GenerateSignature(const Credential& credentials, const string& stringToSign,
                                        const string& simpleDate) const
{
    auto key = ComputeHash(credentials.GetSecretKey(), simpleDate, m_region, m_serviceName);
    return GenerateSignature(stringToSign, key);
}

string JdcloudSignerImpl::GenerateSignature(const string& stringToSign, const string& key) const
{
    LOGSTREAM_DEBUG(logTag, "Final String to sign: \n" << stringToSign);

    stringstream ss;

    auto hashResult = m_hmac->Calculate(stringToSign, key);
    if (!hashResult.IsSuccess())
    {
        LOGSTREAM_ERROR(logTag, "Unable to hmac (sha256) final string");
        LOGSTREAM_DEBUG(logTag, "The final string is: \"" << stringToSign << "\"");
        return "";
    }

    //now we finally sign our request string with our hex encoded derived hash.
    string result = hashResult.GetResult();
    string finalSigningHash = HashingUtils::HexEncode((unsigned char*)result.c_str(), result.length());
    LOGSTREAM_DEBUG(logTag, "Final computed signing hash: " << finalSigningHash);

    return finalSigningHash;
}

string JdcloudSignerImpl::ComputeHash(const string& secretKey, const string& simpleDate, const string& region,
                                const string& serviceName) const
{
    string signingKey(SIGNING_KEY);
    signingKey.append(secretKey);
    auto hashResult = m_hmac->Calculate(simpleDate, signingKey);

    if (!hashResult.IsSuccess())
    {
        LOGSTREAM_ERROR(logTag, "Failed to HMAC (SHA256) date string \"" << simpleDate << "\"");
        return {};
    }

    auto kDate = hashResult.GetResult();
    hashResult = m_hmac->Calculate(region, kDate);
    if (!hashResult.IsSuccess())
    {
        LOGSTREAM_ERROR(logTag, "Failed to HMAC (SHA256) region string \"" << region << "\"");
        return {};
    }

    auto kRegion = hashResult.GetResult();
    hashResult = m_hmac->Calculate(serviceName, kRegion);
    if (!hashResult.IsSuccess())
    {
        LOGSTREAM_ERROR(logTag, "Failed to HMAC (SHA256) service string \"" << m_serviceName << "\"");
        return {};
    }

    auto kService = hashResult.GetResult();
    hashResult = m_hmac->Calculate(JDCLOUD_REQUEST, kService);
    if (!hashResult.IsSuccess())
    {
        LOGSTREAM_ERROR(logTag, "Unable to HMAC (SHA256) request string");
        LOGSTREAM_DEBUG(logTag, "The request string is: \"" << JDCLOUD_REQUEST << "\"");
        return {};
    }
    return hashResult.GetResult();
}

}

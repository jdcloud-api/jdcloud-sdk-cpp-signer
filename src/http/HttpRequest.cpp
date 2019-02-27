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

#include <iostream>
#include <sstream>
#include <algorithm>
#include "util/StringUtils.h"
#include "http/HttpRequest.h"

const char* DATE_HEADER = "x-jdcloud-date";
const char* NONCE_HEADER = "x-jdcloud-nonce";
const char* AUTHORIZATION_HEADER = "authorization";
const char* CONTENT_TYPE_HEADER = "content-type";
const char* USER_AGENT_HEADER = "user-agent";
const char* HOST_HEADER = "host";
const string HttpRequest::m_emptyHeader = "";

static bool IsDefaultPort(const URI& uri)
{
    switch(uri.GetPort())
    {
        case 80:
            return uri.GetScheme() == Scheme::HTTP;
        case 443:
            return uri.GetScheme() == Scheme::HTTPS;
        default:
            return false;
    }
}

HttpRequest::HttpRequest(const URI& uri, HttpMethod method) :
    m_uri(uri),
    m_method(method),
    bodyStream(nullptr)
{
    if(IsDefaultPort(uri))
    {
        SetHeaderValue(HOST_HEADER, uri.GetAuthority());
    }
    else
    {
        std::stringstream host;
        host << uri.GetAuthority() << ":" << uri.GetPort();
        SetHeaderValue(HOST_HEADER, host.str());
    }
}

HeaderValueCollection HttpRequest::GetHeaders() const
{
    HeaderValueCollection headers;

    for (HeaderValueCollection::const_iterator iter = headerMap.begin(); iter != headerMap.end(); ++iter)
    {
        headers.emplace(HeaderValuePair(iter->first, iter->second));
    }

    return headers;
}

const string& HttpRequest::GetHeaderValue(const char* headerName) const
{
    auto iter = headerMap.find(headerName);
    if (iter == headerMap.end())
    {
        return m_emptyHeader;
    }

    return iter->second;
}

void HttpRequest::SetHeaderValue(const char* headerName, const string& headerValue)
{
    headerMap[StringUtils::ToLower(headerName)] = StringUtils::Trim(headerValue.c_str());
}

void HttpRequest::SetHeaderValue(const string& headerName, const string& headerValue)
{
    headerMap[StringUtils::ToLower(headerName.c_str())] = StringUtils::Trim(headerValue.c_str());
}

void HttpRequest::DeleteHeader(const char* headerName)
{
    headerMap.erase(StringUtils::ToLower(headerName));
}

bool HttpRequest::HasHeader(const char* headerName) const
{
    return headerMap.find(StringUtils::ToLower(headerName)) != headerMap.end();
}

int64_t HttpRequest::GetSize() const
{
    int64_t size = 0;

    std::for_each(headerMap.cbegin(), headerMap.cend(), [&](const HeaderValueCollection::value_type& kvPair){ size += kvPair.first.length(); size += kvPair.second.length(); });

    return size;
}

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

#include "jdcloud_signer/http/URI.h"

#include <cstdlib>
#include <cctype>
#include <cassert>
#include <algorithm>
#include <iomanip>
#include <vector>
#include "jdcloud_signer/util/StringUtils.h"

using namespace std;

const char* SEPARATOR = "://";

URI::URI() : m_scheme(Scheme::HTTP), m_port(HTTP_DEFAULT_PORT)
{
}

URI::URI(const string& uri) : m_scheme(Scheme::HTTP), m_port(HTTP_DEFAULT_PORT)
{
    ParseURIParts(uri);
}

URI::URI(const char* uri) : m_scheme(Scheme::HTTP), m_port(HTTP_DEFAULT_PORT)
{
    ParseURIParts(uri);
}

URI& URI::operator =(const string& uri)
{
    this->ParseURIParts(uri);
    return *this;
}

URI& URI::operator =(const char* uri)
{
    this->ParseURIParts(uri);
    return *this;
}

bool URI::operator ==(const URI& other) const
{
    return CompareURIParts(other);
}

bool URI::operator ==(const string& other) const
{
    return CompareURIParts(other);
}

bool URI::operator ==(const char* other) const
{
    return CompareURIParts(other);
}

bool URI::operator !=(const URI& other) const
{
    return !(*this == other);
}

bool URI::operator !=(const string& other) const
{
    return !(*this == other);
}

bool URI::operator !=(const char* other) const
{
    return !(*this == other);
}

void URI::SetScheme(Scheme value)
{
    assert(value == Scheme::HTTP || value == Scheme::HTTPS);

    if (value == Scheme::HTTP)
    {
        m_port = m_port == HTTPS_DEFAULT_PORT || m_port == 0 ? HTTP_DEFAULT_PORT : m_port;
        m_scheme = value;
    }
    else if (value == Scheme::HTTPS)
    {
        m_port = m_port == HTTP_DEFAULT_PORT || m_port == 0 ? HTTPS_DEFAULT_PORT : m_port;
        m_scheme = value;
    }
}

string URI::URLEncodePathRFC3986(const string& path)
{
    if(path.empty())
    {
        return path;
    }

    const vector<string> pathParts = StringUtils::Split(path, '/');
    stringstream ss;
    ss << std::hex << std::uppercase;

    // escape characters appearing in a URL path according to RFC 3986
    for (const auto& segment : pathParts)
    {
        ss << '/';
        for(unsigned char c : segment) // alnum results in UB if the value of c is not unsigned char & is not EOF
        {
            if(std::isalnum(c)) // §2.3 unreserved characters
            {
                ss << c;
                continue;
            }
            switch(c)
            {
                // §2.3 unreserved characters
                case '-': case '_': case '.': case '~':
                // The path section of the URL allow reserved characters to appear unescaped
                // RFC 3986 §2.2 Reserved characters
                // NOTE: this implementation does not accurately implement the RFC on purpose to accommodate for
                // discrepancies in the implementations of URL encoding between services for legacy reasons.
                case '$': case '&': case ',': case '/':
                case ':': case ';': case '=': case '@':
                    ss << c;
                    break;
                default:
                    ss << '%' << std::setw(2) << (int)((unsigned char)c) << std::setw(0);
            }
        }
    }

    //if the last character was also a slash, then add that back here.
    if (path.back() == '/')
    {
        ss << '/';
    }

    return ss.str();
}

string URI::URLEncodePath(const string& path)
{
    vector<string> pathParts = StringUtils::Split(path, '/');
    stringstream ss;

    for (vector<string>::iterator iter = pathParts.begin(); iter != pathParts.end(); ++iter)
    {
        ss << '/' << StringUtils::URLEncode(iter->c_str());
    }

    //if the last character was also a slash, then add that back here.
    if (path[path.length() - 1] == '/')
    {
        ss << '/';
    }

    return ss.str();
}

void URI::SetPath(const string& value)
{
    m_path = value;
}

//ugh, this isn't even part of the canonicalization spec. It is part of how our services have implemented their signers though....
//it doesn't really hurt anything to reorder it though, so go ahead and sort the values for parameters with the same key
void InsertValueOrderedParameter(QueryStringParameterCollection& queryParams, const string& key, const string& value)
{
    auto entriesAtKey = queryParams.equal_range(key);
    for (auto& entry = entriesAtKey.first; entry != entriesAtKey.second; ++entry)
    {
        if (entry->second > value)
        {
            queryParams.emplace_hint(entry, key, value);
            return;
        }
    }

    queryParams.emplace(key, value);
}

QueryStringParameterCollection URI::GetQueryStringParameters(bool decode) const
{
    string queryString = GetQueryString();

    QueryStringParameterCollection parameterCollection;

    //if we actually have a query string
    if (queryString.size() > 0)
    {
        size_t currentPos = 1, locationOfNextDelimiter = 1;

        //while we have params left to parse
        while (currentPos < queryString.size())
        {
            //find next key/value pair
            locationOfNextDelimiter = queryString.find('&', currentPos);

            string keyValuePair;

            //if this isn't the last parameter
            if (locationOfNextDelimiter != string::npos)
            {
                keyValuePair = queryString.substr(currentPos, locationOfNextDelimiter - currentPos);
            }
            //if it is the last parameter
            else
            {
                keyValuePair = queryString.substr(currentPos);
            }

            //split on =
            size_t locationOfEquals = keyValuePair.find('=');
            string key = keyValuePair.substr(0, locationOfEquals);
            string value = keyValuePair.substr(locationOfEquals + 1);

            if(decode)
            {
                InsertValueOrderedParameter(parameterCollection, StringUtils::URLDecode(key.c_str()), StringUtils::URLDecode(value.c_str()));
            }
            else
            {
                InsertValueOrderedParameter(parameterCollection, key, value);
            }

            currentPos += keyValuePair.size() + 1;
        }
    }

    return parameterCollection;
}

void URI::CanonicalizeQueryString()
{
    QueryStringParameterCollection sortedParameters = GetQueryStringParameters(false);
    stringstream queryStringStream;

    bool first = true;

    if(sortedParameters.size() > 0)
    {
        queryStringStream << "?";
    }

    if(m_queryString.find("=") != std::string::npos)
    {
        for (QueryStringParameterCollection::iterator iter = sortedParameters.begin();
            iter != sortedParameters.end(); ++iter)
        {
            if (!first)
            {
                queryStringStream << "&";
            }

            first = false;
            queryStringStream << iter->first.c_str() << "=" << iter->second.c_str();
        }

        m_queryString = queryStringStream.str();
    }
}

void URI::AddQueryStringParameter(const char* key, const string& value)
{
    if (m_queryString.size() <= 0)
    {
        m_queryString.append("?");
    }
    else
    {
        m_queryString.append("&");
    }

    m_queryString.append(StringUtils::URLEncode(key) + "=" + StringUtils::URLEncode(value.c_str()));
}

void URI::AddQueryStringParameter(const map<string, string>& queryStringPairs)
{
    for(const auto& entry: queryStringPairs)
    {
        AddQueryStringParameter(entry.first.c_str(), entry.second);
    }
}

void URI::SetQueryString(const string& str)
{
    m_queryString = "";

    if (str.empty()) return;

    if (str.front() != '?')
    {
        m_queryString.append("?").append(str);
    }
    else
    {
        m_queryString = str;
    }
}

string URI::GetURIString(bool includeQueryString) const
{
    assert(m_authority.size() > 0);

    stringstream ss;
    ss << SchemeMapper::ToString(m_scheme) << SEPARATOR << m_authority;

    if (m_scheme == Scheme::HTTP && m_port != HTTP_DEFAULT_PORT)
    {
        ss << ":" << m_port;
    }
    else if (m_scheme == Scheme::HTTPS && m_port != HTTPS_DEFAULT_PORT)
    {
        ss << ":" << m_port;
    }

    if(m_path != "/")
    {
        ss << URLEncodePathRFC3986(m_path);
    }

    if(includeQueryString)
    {
        ss << m_queryString;
    }

    return ss.str();
}

void URI::ParseURIParts(const string& uri)
{
    ExtractAndSetScheme(uri);
    ExtractAndSetAuthority(uri);
    ExtractAndSetPort(uri);
    ExtractAndSetPath(uri);
    ExtractAndSetQueryString(uri);
}

void URI::ExtractAndSetScheme(const string& uri)
{
    size_t posOfSeparator = uri.find(SEPARATOR);

    if (posOfSeparator != string::npos)
    {
        string schemePortion = uri.substr(0, posOfSeparator);
        SetScheme(SchemeMapper::FromString(schemePortion.c_str()));
    }
    else
    {
        SetScheme(Scheme::HTTP);
    }
}

void URI::ExtractAndSetAuthority(const string& uri)
{
    size_t authorityStart = uri.find(SEPARATOR);

    if (authorityStart == string::npos)
    {
        authorityStart = 0;
    }
    else
    {
        authorityStart += 3;
    }

    size_t posOfEndOfAuthorityPort = uri.find(':', authorityStart);
    size_t posOfEndOfAuthoritySlash = uri.find('/', authorityStart);
    size_t posOfEndOfAuthorityQuery = uri.find('?', authorityStart);
    size_t posEndOfAuthority = (std::min)({posOfEndOfAuthorityPort, posOfEndOfAuthoritySlash, posOfEndOfAuthorityQuery});
    if (posEndOfAuthority == string::npos)
    {
        posEndOfAuthority = uri.length();
    }

    SetAuthority(uri.substr(authorityStart, posEndOfAuthority - authorityStart));
}

void URI::ExtractAndSetPort(const string& uri)
{
    size_t authorityStart = uri.find(SEPARATOR);

    if(authorityStart == string::npos)
    {
        authorityStart = 0;
    }
    else
    {
        authorityStart += 3;
    }

    size_t positionOfPortDelimiter = uri.find(':', authorityStart);

    bool hasPort = positionOfPortDelimiter != string::npos;

    if ((uri.find('/', authorityStart) < positionOfPortDelimiter) || (uri.find('?', authorityStart) < positionOfPortDelimiter))
    {
        hasPort = false;
    }

    if (hasPort)
    {
        string strPort;

        size_t i = positionOfPortDelimiter + 1;
        char currentDigit = uri[i];

        while (std::isdigit(currentDigit))
        {
            strPort += currentDigit;
            currentDigit = uri[++i];
        }

        SetPort(static_cast<uint16_t>(atoi(strPort.c_str())));
    }
}

void URI::ExtractAndSetPath(const string& uri)
{
    size_t authorityStart = uri.find(SEPARATOR);

    if (authorityStart == string::npos)
    {
        authorityStart = 0;
    }
    else
    {
        authorityStart += 3;
    }

    size_t pathEnd = uri.find('?');

    if (pathEnd == string::npos)
    {
        pathEnd = uri.length();
    }

    string authorityAndPath = uri.substr(authorityStart, pathEnd - authorityStart);

    size_t pathStart = authorityAndPath.find('/');

    if (pathStart != string::npos)
    {
        SetPath(authorityAndPath.substr(pathStart, pathEnd - pathStart));
    }
    else
    {
        SetPath("/");
    }
}

void URI::ExtractAndSetQueryString(const string& uri)
{
    size_t queryStart = uri.find('?');

    if (queryStart != string::npos)
    {
        m_queryString = uri.substr(queryStart);
    }
}

string URI::GetFormParameters() const
{
    if(m_queryString.length() == 0)
    {
        return "";
    }
    else
    {
        return m_queryString.substr(1);
    }
}

bool URI::CompareURIParts(const URI& other) const
{
    return m_scheme == other.m_scheme && m_authority == other.m_authority && m_path == other.m_path && m_queryString == other.m_queryString;
}

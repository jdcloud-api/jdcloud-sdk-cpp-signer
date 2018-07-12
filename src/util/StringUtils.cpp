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

#include <algorithm>
#include <iomanip>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <functional>
#include "util/StringUtils.h"

#ifdef _WIN32
#include <Windows.h>
#endif


void StringUtils::Replace(string& s, const char* search, const char* replace)
{
    if(!search || !replace)
    {
        return;
    }

    size_t replaceLength = strlen(replace);
    size_t searchLength = strlen(search);

    for (std::size_t pos = 0;; pos += replaceLength)
    {
        pos = s.find(search, pos);
        if (pos == string::npos)
            break;

        s.erase(pos, searchLength);
        s.insert(pos, replace);
    }
}

string StringUtils::ToLower(const char* source)
{
    string copy;
    size_t sourceLength = strlen(source);
    copy.resize(sourceLength);
    //appease the latest whims of the VC++ 2017 gods
    std::transform(source, source + sourceLength, copy.begin(), [](unsigned char c) { return (char)::tolower(c); });

    return copy;
}

string StringUtils::ToUpper(const char* source)
{
    string copy;
    size_t sourceLength = strlen(source);
    copy.resize(sourceLength);
    //appease the latest whims of the VC++ 2017 gods
    std::transform(source, source + sourceLength, copy.begin(), [](unsigned char c) { return (char)::toupper(c); });

    return copy;
}


bool StringUtils::CaselessCompare(const char* value1, const char* value2)
{
    string value1Lower = ToLower(value1);
    string value2Lower = ToLower(value2);

    return value1Lower == value2Lower;
}

vector<string> StringUtils::Split(const string& toSplit, char splitOn)
{
    return Split(toSplit, splitOn, SIZE_MAX);
}

vector<string> StringUtils::Split(const string& toSplit, char splitOn, size_t numOfTargetParts)
{
    vector<string> returnValues;
    stringstream input(toSplit);
    string item;

    while(returnValues.size() < numOfTargetParts - 1 && std::getline(input, item, splitOn))
    {
        if (item.size())
        {
            returnValues.emplace_back(std::move(item));
        }
    }

    if (std::getline(input, item, static_cast<char>(EOF)) && item.size())
    {
        returnValues.emplace_back(std::move(item));
    }
    return returnValues;
}

vector<string> StringUtils::SplitOnLine(const string& toSplit)
{
    stringstream input(toSplit);
    vector<string> returnValues;
    string item;

    while (std::getline(input, item))
    {
        if (item.size() > 0)
        {
            returnValues.push_back(item);
        }
    }

    return returnValues;
}


string StringUtils::URLEncode(const char* unsafe)
{
    stringstream escaped;
    escaped.fill('0');
    escaped << std::hex << std::uppercase;

    size_t unsafeLength = strlen(unsafe);
    for (auto i = unsafe, n = unsafe + unsafeLength; i != n; ++i)
    {
        int c = *i;
		//MSVC 2015 has an assertion that c is positive in isalnum(). This breaks unicode support.
		//bypass that with the first check.
        if (c >= 0 && (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~'))
        {
            escaped << (char)c;
        }
        else
        {
            //this unsigned char cast allows us to handle unicode characters.
            escaped << '%' << std::setw(2) << int((unsigned char)c) << std::setw(0);
        }
    }

    return escaped.str();
}

string StringUtils::UTF8Escape(const char* unicodeString, const char* delimiter)
{
    stringstream escaped;
    escaped.fill('0');
    escaped << std::hex << std::uppercase;

    size_t unsafeLength = strlen(unicodeString);
    for (auto i = unicodeString, n = unicodeString + unsafeLength; i != n; ++i)
    {
        int c = *i;
        //MSVC 2015 has an assertion that c is positive in isalnum(). This breaks unicode support.
        //bypass that with the first check.
        if (c >= ' ' && c < 127 )
        {
            escaped << (char)c;
        }
        else
        {
            //this unsigned char cast allows us to handle unicode characters.
            escaped << delimiter << std::setw(2) << int((unsigned char)c) << std::setw(0);
        }
    }

    return escaped.str();
}

string StringUtils::URLEncode(double unsafe)
{
    char buffer[32];
#if defined(_MSC_VER) && _MSC_VER < 1900
    _snprintf_s(buffer, sizeof(buffer), _TRUNCATE, "%g", unsafe);
#else
    snprintf(buffer, sizeof(buffer), "%g", unsafe);
#endif

    return StringUtils::URLEncode(buffer);
}


string StringUtils::URLDecode(const char* safe)
{
    stringstream unescaped;
    unescaped.fill('0');
    unescaped << std::hex;

    size_t safeLength = strlen(safe);
    for (auto i = safe, n = safe + safeLength; i != n; ++i)
    {
        char c = *i;
        if(c == '%')
        {
            char hex[3];
            hex[0] = *(i + 1);
            hex[1] = *(i + 2);
            hex[2] = 0;
            i += 2;
            auto hexAsInteger = strtol(hex, nullptr, 16);
            unescaped << (char)hexAsInteger;
        }
        else
        {
            unescaped << *i;
        }
    }

    return unescaped.str();
}

string StringUtils::LTrim(const char* source)
{
    string copy(source);
    copy.erase(copy.begin(), std::find_if(copy.begin(), copy.end(), [](int ch) { return !::isspace(ch); }));
    return copy;
}

// trim from end
string StringUtils::RTrim(const char* source)
{
    string copy(source);
    copy.erase(std::find_if(copy.rbegin(), copy.rend(), [](int ch) { return !::isspace(ch); }).base(), copy.end());
    return copy;
}

// trim from both ends
string StringUtils::Trim(const char* source)
{
    return LTrim(RTrim(source).c_str());
}

long long StringUtils::ConvertToInt64(const char* source)
{
    if(!source)
    {
        return 0;
    }

#ifdef __ANDROID__
    return atoll(source);
#else
    return std::atoll(source);
#endif // __ANDROID__
}


long StringUtils::ConvertToInt32(const char* source)
{
    if (!source)
    {
        return 0;
    }

    return std::atol(source);
}


bool StringUtils::ConvertToBool(const char* source)
{
    if(!source)
    {
        return false;
    }

    string strValue = ToLower(source);
    if(strValue == "true" || strValue == "1")
    {
        return true;
    }

    return false;
}


double StringUtils::ConvertToDouble(const char* source)
{
    if(!source)
    {
        return 0.0;
    }

    return std::strtod(source, NULL);
}

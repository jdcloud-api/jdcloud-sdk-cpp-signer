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

#include <time.h>
#include <cassert>
#include <iostream>
#include <cstring>
#include "util/DateTime.h"

static const char* RFC822_DATE_FORMAT_STR_WITH_Z = "%a, %d %b %Y %H:%M:%S %Z";
static const char* ISO_8601_LONG_DATE_FORMAT_STR = "%Y-%m-%dT%H:%M:%SZ";

DateTime::DateTime(const std::chrono::system_clock::time_point& timepointToAssign) : m_time(timepointToAssign), m_valid(true)
{
}

DateTime::DateTime(int64_t millisSinceEpoch) : m_valid(true)
{
    std::chrono::duration<int64_t, std::chrono::milliseconds::period> timestamp(millisSinceEpoch);
    m_time = std::chrono::system_clock::time_point(timestamp);
}

DateTime::DateTime(double epoch_millis) : m_valid(true)
{
    std::chrono::duration<double, std::chrono::seconds::period> timestamp(epoch_millis);
    m_time = std::chrono::system_clock::time_point(std::chrono::duration_cast<std::chrono::milliseconds>(timestamp));
}

DateTime::DateTime() : m_valid(true)
{
    //init time_point to default by doing nothing.
}

DateTime& DateTime::operator=(double secondsMillis)
{
    *this = DateTime(secondsMillis);
    return *this;
}

DateTime& DateTime::operator=(int64_t millisSinceEpoch)
{
    *this = DateTime(millisSinceEpoch);
    return *this;
}

DateTime& DateTime::operator=(const std::chrono::system_clock::time_point& timepointToAssign)
{
    *this = DateTime(timepointToAssign);
    return *this;
}

bool DateTime::operator == (const DateTime& other) const
{
    return m_time == other.m_time;
}

bool DateTime::operator < (const DateTime& other) const
{
    return m_time < other.m_time;
}

bool DateTime::operator > (const DateTime& other) const
{
    return m_time > other.m_time;
}

bool DateTime::operator != (const DateTime& other) const
{
    return m_time != other.m_time;
}

bool DateTime::operator <= (const DateTime& other) const
{
    return m_time <= other.m_time;
}

bool DateTime::operator >= (const DateTime& other) const
{
    return m_time >= other.m_time;
}

DateTime DateTime::operator +(const std::chrono::milliseconds& a) const
{
    auto timepointCpy = m_time;
    timepointCpy += a;
    return DateTime(timepointCpy);
}

DateTime DateTime::operator -(const std::chrono::milliseconds& a) const
{
    auto timepointCpy = m_time;
    timepointCpy -= a;
    return DateTime(timepointCpy);
}

string DateTime::ToLocalTimeString(DateFormat format) const
{
    switch (format)
    {
    case DateFormat::ISO_8601:
        return ToLocalTimeString(ISO_8601_LONG_DATE_FORMAT_STR);
    case DateFormat::RFC822:
        return ToLocalTimeString(RFC822_DATE_FORMAT_STR_WITH_Z);
    default:
        assert(0);
        return "";
    }
}

string DateTime::ToLocalTimeString(const char* formatStr) const
{
    struct tm localTimeStamp = ConvertTimestampToLocalTimeStruct();

    char formattedString[100];
    std::strftime(formattedString, sizeof(formattedString), formatStr, &localTimeStamp);
    return formattedString;
}

string DateTime::ToGmtString(const char* formatStr) const
{
    struct tm gmtTimeStamp = ConvertTimestampToGmtStruct();

    char formattedString[100];
    std::strftime(formattedString, sizeof(formattedString), formatStr, &gmtTimeStamp);
    return formattedString;
}

double DateTime::SecondsWithMSPrecision() const
{
    std::chrono::duration<double, std::chrono::seconds::period> timestamp(m_time.time_since_epoch());
    return timestamp.count();
}

int64_t DateTime::Millis() const
{
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(m_time.time_since_epoch());
    return timestamp.count();
}

std::chrono::system_clock::time_point DateTime::UnderlyingTimestamp() const
{
    return m_time;
}

bool DateTime::IsDST(bool localTime) const
{
    return GetTimeStruct(localTime).tm_isdst == 0 ? false : true;
}

DateTime DateTime::Now()
{
    DateTime dateTime;
    dateTime.m_time = std::chrono::system_clock::now();
    return dateTime;
}

std::chrono::milliseconds DateTime::Diff(const DateTime& a, const DateTime& b)
{
    auto diff = a.m_time - b.m_time;
    return std::chrono::duration_cast<std::chrono::milliseconds>(diff);
}

tm DateTime::GetTimeStruct(bool localTime) const
{
    return localTime ? ConvertTimestampToLocalTimeStruct() : ConvertTimestampToGmtStruct();
}

tm DateTime::ConvertTimestampToLocalTimeStruct() const
{
    std::time_t time = std::chrono::system_clock::to_time_t(m_time);
    struct tm localTimeStamp;
#ifdef WIN32
    localtime_s(&localTimeStamp, &time);
#else
    localtime_r(&time, &localTimeStamp);
#endif

    return localTimeStamp;
}

tm DateTime::ConvertTimestampToGmtStruct() const
{
    std::time_t time = std::chrono::system_clock::to_time_t(m_time);
    struct tm gmtTimeStamp;
#ifdef WIN32
    localtime_s(&gmtTimeStamp, &time);
#else
    gmtime_r(&time, &gmtTimeStamp);
#endif

    return gmtTimeStamp;
}

string DateTime::CalculateGmtTimestampAsString(const char* formatStr)
{
    DateTime now = Now();
    return now.ToGmtString(formatStr);
}

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

#include <chrono>
#include <string>

using std::string;

enum class DateFormat
{
    RFC822, //for http headers
    ISO_8601, //for query and xml payloads
    AutoDetect
};

enum class Month
{
    January = 0,
    February,
    March,
    April,
    May,
    June,
    July,
    August,
    September,
    October,
    November,
    December
};

enum class DayOfWeek
{
    Sunday = 0,
    Monday,
    Tuesday,
    Wednesday,
    Thursday,
    Friday,
    Saturday
};

/**
 * Wrapper for all the weird crap we need to do with timestamps.
 */
class DateTime
{
public:
    /**
     *  Initializes time point to epoch
     */
    DateTime();

    /**
    *  Initializes time point to any other arbirtrary timepoint
    */
    DateTime(const std::chrono::system_clock::time_point& timepointToAssign);

    /**
     * Initializes time point to millis Since epoch
     */
    DateTime(int64_t millisSinceEpoch);

    /**
     * Initializes time point to epoch time in seconds.millis
     */
    DateTime(double epoch_millis);

    bool operator == (const DateTime& other) const;
    bool operator < (const DateTime& other) const;
    bool operator > (const DateTime& other) const;
    bool operator != (const DateTime& other) const;
    bool operator <= (const DateTime& other) const;
    bool operator >= (const DateTime& other) const;

    DateTime operator+(const std::chrono::milliseconds& a) const;
    DateTime operator-(const std::chrono::milliseconds& a) const;

    /**
     * Assign from seconds.millis since epoch.
     */
    DateTime& operator=(double secondsSinceEpoch);

    /**
     * Assign from millis since epoch.
     */
    DateTime& operator=(int64_t millisSinceEpoch);

    /**
    * Assign from another time_point
    */
    DateTime& operator=(const std::chrono::system_clock::time_point& timepointToAssign);

    /**
     * Whether or not parsing the timestamp from string was successful.
     */
    inline bool WasParseSuccessful() const { return m_valid; }

    /**
     * Convert dateTime to local time string using predefined format.
     */
    string ToLocalTimeString(DateFormat format) const;

    /**
    * Convert dateTime to local time string using arbitrary format.
    */
    string ToLocalTimeString(const char* formatStr) const;

    /**
    * Convert dateTime to GMT time string using arbitrary format.
    */
    string ToGmtString(const char* formatStr) const;

    /**
     * Get the representation of this datetime as seconds.milliseconds since epoch
     */
    double SecondsWithMSPrecision() const;

    /**
     * Milliseconds since epoch of this datetime.
     */
    int64_t Millis() const;

    /**
     *  In the likely case this class doesn't do everything you need to do, here's a copy of the time_point structure. Have fun.
     */
    std::chrono::system_clock::time_point UnderlyingTimestamp() const;

    /**
    * Get whether or not this dateTime is in Daylight savings time. localTime if true, return local time, otherwise return UTC
    */
    bool IsDST(bool localTime = false) const;

    /**
     * Get an instance of DateTime representing this very instant.
     */
    static DateTime Now();

    /**
     * Compute the difference between two timestamps.
     */
    static std::chrono::milliseconds Diff(const DateTime& a, const DateTime& b);

    /**
     * Calculates the current gmt timestamp, formats it, and returns it as a string
     */
    static string CalculateGmtTimestampAsString(const char* formatStr);

private:
    std::chrono::system_clock::time_point m_time;
    bool m_valid;

    tm GetTimeStruct(bool localTime) const;
    tm ConvertTimestampToLocalTimeStruct() const;
    tm ConvertTimestampToGmtStruct() const;
};

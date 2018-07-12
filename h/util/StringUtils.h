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
#include <vector>
#include <sstream>

using std::string;
using std::vector;
using std::stringstream;

/**
* All the things the c++ stdlib is missing for string operations that I needed.
*/
class StringUtils
{
public:
    static void Replace(string& s, const char* search, const char* replace);
    
    /**
    * Converts a string to lower case.
    */
    static string ToLower(const char* source);


    /**
    * Converts a string to upper case.
    */
    static string ToUpper(const char* source);


    /**
    * Does a caseless comparison of two strings.
    */
    static bool CaselessCompare(const char* value1, const char* value2);


    /**
    * URL encodes a string (uses %20 not + for spaces).
    */
    static string URLEncode(const char* unsafe);

    /**
    * Http Clients tend to escape some characters but not all. Escaping all of them causes problems, because the client
    * will also try to escape them.
    * So this only escapes non-ascii characters and the + character
    */
    static string UTF8Escape(const char* unicodeString, const char* delimiter);

    /**
    * URL encodes a double (if it ends up going to scientific notation) otherwise it just returns it as a string.
    */
    static string URLEncode(double unsafe);


    /**
    * Decodes a URL encoded string (will handle both encoding schemes for spaces).
    */
    static string URLDecode(const char* safe);

    /**
     * @brief Splits a string on a delimiter (empty items are excluded).
     * @param toSplit, the original string to split
     * @param splitOn, the delemiter you want to use.
     */
    static vector<string> Split(const string& toSplit, char splitOn);

    /**
     * @brief Splits a string on a delimiter (empty items are excluded).
     * @param toSplit, the original string to split
     * @param splitOn, the delemiter you want to use.
     * @param numOfTargetParts, how many target parts you want to get, if it is 0, as many as possible.
     */
    static vector<string> Split(const string& toSplit, char splitOn, size_t numOfTargetParts);


    /**
    * Splits a string on new line characters.
    */
    static vector<string> SplitOnLine(const string& toSplit);


    /** static vector<string> SplitOnRegex(string regex);
     *  trim from start
     */
    static string LTrim(const char* source);


    /**
     * trim from end
     */
    static string RTrim(const char* source);

    /**
     * trim from both ends
     */
    static string Trim(const char* source);


    /**
     * convert to int 64
     */
    static long long ConvertToInt64(const char* source);


    /**
     * convert to int 32
     */
    static long ConvertToInt32(const char* source);


    /** 
     * convert to bool
     */
    static bool ConvertToBool(const char* source);


    /**
     * convert to double
     */
    static double ConvertToDouble(const char* source);

};

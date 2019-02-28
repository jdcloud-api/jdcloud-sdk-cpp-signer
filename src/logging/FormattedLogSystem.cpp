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

#include "jdcloud_signer/logging/FormattedLogSystem.h"

#include "jdcloud_signer/util/DateTime.h"

#include <fstream>
#include <cstdarg>
#include <stdio.h>
#include <thread>

using namespace std;

namespace jdcloud_signer {

static string CreateLogPrefixLine(LogLevel logLevel, const char* tag)
{
    stringstream ss;

    switch(logLevel)
    {
        case LogLevel::Error:
            ss << "[ERROR] ";
            break;

        case LogLevel::Fatal:
            ss << "[FATAL] ";
            break;

        case LogLevel::Warn:
            ss << "[WARN] ";
            break;

        case LogLevel::Info:
            ss << "[INFO] ";
            break;

        case LogLevel::Debug:
            ss << "[DEBUG] ";
            break;

        case LogLevel::Trace:
            ss << "[TRACE] ";
            break;

        default:
            ss << "[UNKOWN] ";
            break;
    }

    ss << DateTime::CalculateGmtTimestampAsString("%Y-%m-%d %H:%M:%S") << " " << tag << " [" << std::this_thread::get_id() << "] ";

    return ss.str();
}

FormattedLogSystem::FormattedLogSystem(LogLevel logLevel) :
    m_logLevel(logLevel)
{
}

void FormattedLogSystem::LogStream(LogLevel logLevel, const char* tag, const ostringstream &message_stream)
{
    stringstream ss;
    ss << CreateLogPrefixLine(logLevel, tag) << message_stream.rdbuf()->str() << std::endl;

    ProcessFormattedStatement(ss.str());
}

}

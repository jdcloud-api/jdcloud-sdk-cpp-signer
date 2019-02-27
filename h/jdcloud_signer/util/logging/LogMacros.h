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

#include "LogLevel.h"
#include "Logging.h"
#include "LogSystemInterface.h"

namespace jdcloud_signer {

// While macros are usually grotty, using them here lets us have a simple function call interface for logging that
//
//  (1) Can be compiled out completely, so you don't even have to pay the cost to check the log level (which will be a virtual function call and a std::atomic<> read) if you don't want any logging
//  (2) If you use logging and the log statement doesn't pass the conditional log filter level, not only do you not pay the cost of building the log string, you don't pay the cost for allocating or
//      getting any of the values used in building the log string, as they're in a scope (if-statement) that never gets entered.

#ifdef DISABLE_LOGGING

    #define LOGSTREAM(level, tag, streamExpression)
    #define LOGSTREAM_FATAL(tag, streamExpression)
    #define LOGSTREAM_ERROR(tag, streamExpression)
    #define LOGSTREAM_WARN(tag, streamExpression)
    #define LOGSTREAM_INFO(tag, streamExpression)
    #define LOGSTREAM_DEBUG(tag, streamExpression)
    #define LOGSTREAM_TRACE(tag, streamExpression)

#else

    #define LOGSTREAM(level, tag, streamExpression) \
        { \
            LogSystemInterface* logSystem = GetLogSystem(); \
            if ( logSystem && logSystem->GetLogLevel() >= level ) \
            { \
                ostringstream logStream; \
                logStream << streamExpression; \
                logSystem->LogStream( logLevel, tag, logStream ); \
            } \
        }

    #define LOGSTREAM_FATAL(tag, streamExpression) \
        { \
            LogSystemInterface* logSystem = GetLogSystem(); \
            if ( logSystem && logSystem->GetLogLevel() >= LogLevel::Fatal ) \
            { \
                ostringstream logStream; \
                logStream << streamExpression; \
                logSystem->LogStream( LogLevel::Fatal, tag, logStream ); \
            } \
        }

    #define LOGSTREAM_ERROR(tag, streamExpression) \
        { \
            LogSystemInterface* logSystem = GetLogSystem(); \
            if ( logSystem && logSystem->GetLogLevel() >= LogLevel::Error ) \
            { \
                ostringstream logStream; \
                logStream << streamExpression; \
                logSystem->LogStream( LogLevel::Error, tag, logStream ); \
            } \
        }

    #define LOGSTREAM_WARN(tag, streamExpression) \
        { \
            LogSystemInterface* logSystem = GetLogSystem(); \
            if ( logSystem && logSystem->GetLogLevel() >= LogLevel::Warn ) \
            { \
                ostringstream logStream; \
                logStream << streamExpression; \
                logSystem->LogStream( LogLevel::Warn, tag, logStream ); \
            } \
        }

    #define LOGSTREAM_INFO(tag, streamExpression) \
        { \
            LogSystemInterface* logSystem = GetLogSystem(); \
            if ( logSystem && logSystem->GetLogLevel() >= LogLevel::Info ) \
            { \
                ostringstream logStream; \
                logStream << streamExpression; \
                logSystem->LogStream( LogLevel::Info, tag, logStream ); \
            } \
        }

    #define LOGSTREAM_DEBUG(tag, streamExpression) \
        { \
            LogSystemInterface* logSystem = GetLogSystem(); \
            if ( logSystem && logSystem->GetLogLevel() >= LogLevel::Debug ) \
            { \
                ostringstream logStream; \
                logStream << streamExpression; \
                logSystem->LogStream( LogLevel::Debug, tag, logStream ); \
            } \
        }

    #define LOGSTREAM_TRACE(tag, streamExpression) \
        { \
            LogSystemInterface* logSystem = GetLogSystem(); \
            if ( logSystem && logSystem->GetLogLevel() >= LogLevel::Trace ) \
            { \
                ostringstream logStream; \
                logStream << streamExpression; \
                logSystem->LogStream( LogLevel::Trace, tag, logStream ); \
            } \
        }

#endif // DISABLE_LOGGING

}

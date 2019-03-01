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

#include "jdcloud_signer/logging/LogSystemInterface.h"
#include "jdcloud_signer/logging/LogLevel.h"

#include <atomic>
#include <string>
#include <ostream>
#include <sstream>

namespace jdcloud_signer {

/**
 * Logger that formats log messages into [LEVEL] timestamp [threadid] message
 */
class FormattedLogSystem : public LogSystemInterface
{
public:
    using Base = LogSystemInterface;

    /**
     * Initializes log system with logLevel
     */
    FormattedLogSystem(LogLevel logLevel);
    virtual ~FormattedLogSystem() = default;

    /**
     * Gets the currently configured log level.
     */
    LogLevel GetLogLevel(void) const override { return m_logLevel; }
    /**
     * Set a new log level. This has the immediate effect of changing the log output to the new level.
     */
    void SetLogLevel(LogLevel logLevel) { m_logLevel.store(logLevel); }

    /**
     * Writes the stream to ProcessFormattedStatement.
     */
    void LogStream(LogLevel logLevel, const char* tag, const std::ostringstream &messageStream) override;

protected:
    /**
     * This is the method that most logger implementations will want to override.
     * At this point the message is formatted and is ready to go to the output stream
     */
    virtual void ProcessFormattedStatement(std::string&& statement) = 0;

private:
    std::atomic<LogLevel> m_logLevel;
};

}

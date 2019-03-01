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

#include <ostream>

namespace jdcloud_signer {

enum class LogLevel : int;

/**
 * Interface for logging implementations. If you want to write your own logger, you can start here, though you may have more
 * luck going down one more level to FormattedLogSystem. It does a bit more of the work for you and still gives you the ability
 * to override the IO portion.
 */
class LogSystemInterface
{
public:
    virtual ~LogSystemInterface() = default;

    /**
     * Gets the currently configured log level for this logger.
     */
    virtual LogLevel GetLogLevel(void) const = 0;
    /**
     * Writes the stream to the output stream.
     */
    virtual void LogStream(LogLevel logLevel, const char* tag, const std::ostringstream &messageStream) = 0;
};

}

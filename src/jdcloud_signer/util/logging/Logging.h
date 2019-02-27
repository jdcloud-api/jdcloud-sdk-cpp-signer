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

#include <memory>

namespace jdcloud_signer {

class LogSystemInterface;

// Standard interface

/**
 * Call this at the beginning of your program, prior to any calls.
 */
void InitializeLogging(const std::shared_ptr<LogSystemInterface>& logSystem);

/**
 * Call this at the exit point of your program, after all calls have finished.
 */
void ShutdownLogging(void);

/**
 * Get currently configured log system instance.
 */
LogSystemInterface* GetLogSystem();

}

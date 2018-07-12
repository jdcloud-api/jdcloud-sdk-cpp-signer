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

using std::string;

class Credential
{
public:
    Credential(const string& accessKey, const string& secretKey):
        m_accessKey(accessKey), m_secretKey(secretKey)
    {}

    inline const string& GetAccessKey() const
    {
        return m_accessKey;
    }

    inline const string& GetSecretKey() const
    {
        return m_secretKey;
    }

private:
    std::string m_accessKey;
    std::string m_secretKey;
};

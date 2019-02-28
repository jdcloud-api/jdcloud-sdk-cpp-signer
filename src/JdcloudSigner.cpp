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

#include "jdcloud_signer/JdcloudSigner.h"

#include "jdcloud_signer/JdcloudSignerImpl.h"

using namespace std;

namespace jdcloud_signer {

JdcloudSigner::JdcloudSigner(const Credential& credential, const string& serviceName, const string& region) :
    m_credential(credential),
    m_serviceName(serviceName),
    m_region(region)
{
}

JdcloudSigner::~JdcloudSigner()
{
}

bool JdcloudSigner::SignRequest(HttpRequest& request) const
{
    JdcloudSignerImpl impl(m_credential, m_serviceName, m_region);
    return impl.SignRequest(request);
}

}

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

#include "jdcloud_signer/http/Scheme.h"
#include "jdcloud_signer/util/StringUtils.h"

using namespace std;

namespace jdcloud_signer {

namespace SchemeMapper
{

    const char* ToString(Scheme scheme)
    {
        switch (scheme)
        {
            case Scheme::HTTP:
                return "http";
            case Scheme::HTTPS:
                return "https";
            default:
                return "http";
        }
    }

    Scheme FromString(const char* name)
    {
        string trimmedString = StringUtils::Trim(name);
        string loweredTrimmedString = StringUtils::ToLower(trimmedString.c_str());

        if (loweredTrimmedString == "http")
        {
            return Scheme::HTTP;
        }
        //this branch is technically unneeded, but it is here so we don't have a subtle bug
        //creep in as we extend this enum.
        else if (loweredTrimmedString == "https")
        {
            return Scheme::HTTPS;
        }

        return Scheme::HTTPS;
    }

} // namespace SchemeMapper

}

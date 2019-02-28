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

#include <iostream>
#include <memory>
#include "jdcloud_signer/http/URI.h"
#include "jdcloud_signer/http/HttpTypes.h"

extern const char* DATE_HEADER;
extern const char* NONCE_HEADER;
extern const char* AUTHORIZATION_HEADER;
extern const char* CONTENT_TYPE_HEADER;
extern const char* USER_AGENT_HEADER;
extern const char* HOST_HEADER;

class HttpRequest
{
public:
    /**
     * Initializes an HttpRequest object with uri and http method.
     */
    HttpRequest(const URI& uri, HttpMethod method);

    /**
     * Gets underlying URI object with mutation access.
     */
    inline URI& GetUri()
    {
        return m_uri;
    }
    /**
     * Gets the underlying URI object.
     */
    const URI& GetUri() const { return m_uri; }
    /**
     * Converts the URI into a string and returns it. If includeQueryString is set to true, the query string
     * will be included in the returned value.
     */
    inline std::string GetURIString(bool includeQueryString = true) const
    {
        return m_uri.GetURIString(includeQueryString);
    }
    /**
     * Get the http method for this request.
     */
    inline HttpMethod GetMethod() const
    {
        return m_method;
    }
    /**
     * Gets the query string from the URI on this request.
     */
    inline const std::string& GetQueryString() const
    {
        return m_uri.GetQueryString();
    }
    /**
     * Get All headers for this request.
     */
    HeaderValueCollection GetHeaders() const;
    /**
     * Get the value for a Header based on its name.
     */
    const std::string& GetHeaderValue(const char* headerName) const;
    /**
     * Add a header pair
     */
    void SetHeaderValue(const char* headerName, const std::string& headerValue);
    /**
     * Add a header pair
     */
    void SetHeaderValue(const std::string& headerName, const std::string& headerValue);
    /**
     * delete pair by headerName
     */
    void DeleteHeader(const char* headerName);
    /**
     * Adds a content body stream to the request. This stream will be used to send the body to the endpoint.
     */
    inline void AddContentBody(const std::shared_ptr<std::iostream>& strContent) { bodyStream = strContent; }
    /**
     * Gets the content body stream that will be used for this request.
     */
    inline const std::shared_ptr<std::iostream>& GetContentBody() const { return bodyStream; }
    /**
     * Returns true if a header exists in the request with name
     */
    bool HasHeader(const char*) const;
    /**
     * Get size in bytes of the request when as it will be going accross the wire.
     */
    int64_t GetSize() const;
    /**
     * Normalizes the URI for use with signing.
     */
    inline void CanonicalizeRequest()
    {
        m_uri.CanonicalizeQueryString();
    }
    /**
     * Sets authorization header.
     */
    inline void SetAuthorization(const std::string& value)
    {
        SetHeaderValue(AUTHORIZATION_HEADER, value);
    }

private:
    URI m_uri;
    HttpMethod m_method;
    HeaderValueCollection headerMap;
    std::shared_ptr<std::iostream> bodyStream;
    static const std::string m_emptyHeader;
};

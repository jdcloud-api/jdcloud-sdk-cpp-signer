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

#include <stdint.h>
#include <string>
#include <map>
#include "jdcloud_signer/http/Scheme.h"

namespace jdcloud_signer {

extern const char* SEPARATOR;
static const uint16_t HTTP_DEFAULT_PORT = 80;
static const uint16_t HTTPS_DEFAULT_PORT = 443;

//per https://tools.ietf.org/html/rfc3986#section-3.4 there is nothing preventing servers from allowing
//multiple values for the same key. So use a multimap instead of a map.
typedef std::map<std::string, std::string> QueryStringParameterCollection;

/**
 * class modeling universal resource identifier, but implemented for http
 */
class URI
{
public:
    /**
     * Defaults to http and port 80
     */
    URI();
    /**
     * Parses string and sets uri fields
     */
    URI(const std::string&);
    /**
     * Parses string and sets uri fields
     */
    URI(const char*);

    URI& operator = (const std::string&);
    URI& operator = (const char*);

    bool operator == (const URI&) const;
    bool operator == (const std::string&) const;
    bool operator == (const char*) const;
    bool operator != (const URI&) const;
    bool operator != (const std::string&) const;
    bool operator != (const char*) const;

    /**
     * scheme or protocol e.g. http, https, ftp
     */
    inline Scheme GetScheme() const { return m_scheme; }

    /**
     * Sets scheme, if the port is incompaitible with this scheme, the port will automatically be set as well.
     */
    void SetScheme(Scheme value);

    /**
     * Gets the domain portion of the uri
     */
    inline const std::string& GetAuthority() const { return m_authority; }

    /**
     * Sets the domain portion of the uri
     */
    inline void SetAuthority(const std::string& value) { m_authority = value; }

    /**
     * Gets the port portion of the uri, defaults to 22 for ftp, 80 for http and 443 for https
     */
    inline uint16_t GetPort() const { return m_port; }

    /**
     * Sets the port portion of the uri, normally you will not have to do this. If the scheme is set to ftp, http
     * or https then the default ports will be set.
     */
    inline void SetPort(uint16_t value) { m_port = value; }

    /**
     * Gets the path portion of the uri e.g. the portion after the first slash after the authority and prior to the
     * query string. This is not url encoded.
     */
    inline const std::string& GetPath() const { return m_path; }

    /**
     * Gets the path portion of the uri, url encodes it and returns it
     */
    inline std::string GetURLEncodedPath() const { return URLEncodePath(m_path); }

    /**
     * Sets the path portion of the uri. URL encodes it if needed
     */
    void SetPath(const std::string& value);

    /**
     * Gets the raw query string including the ?
     */
    inline const std::string& GetQueryString() const { return m_queryString; }

    /**
     * Resets the query string to the raw string. all query string manipulations made before this call will be lost
     */
    void SetQueryString(const std::string& str);

    std::string GetFormParameters() const;

    /**
     * Cannonicalizes the query string.
     */
    void CanonicalizeQueryString();

    /**
     * parses query string and returns url decoded key/value mappings from it. Spaces and all url encoded
     * values will not be encoded.
     */
    QueryStringParameterCollection GetQueryStringParameters(bool decode = true) const;

    /**
     * Adds query string parameter to underlying query string.
     */
    void AddQueryStringParameter(const char* key, const std::string& value);

    /**
     * Adds multiple query string parameters to underlying query string.
     */
    void AddQueryStringParameter(const std::map<std::string, std::string>& queryStringPairs);

    /**
     * Converts the URI to a String usable for its context. e.g. an http request.
     */
    std::string GetURIString(bool includeQueryString = true) const;

    /**
     * URLEncodes the path portions of path (doesn't encode the "/" portion)
     */
    static std::string URLEncodePath(const std::string& path);

    /**
     * URLEncodes the path portion of the URI according to RFC3986
     */
    static std::string URLEncodePathRFC3986(const std::string& path);


private:
    void ParseURIParts(const std::string& uri);
    void ExtractAndSetScheme(const std::string& uri);
    void ExtractAndSetAuthority(const std::string& uri);
    void ExtractAndSetPort(const std::string& uri);
    void ExtractAndSetPath(const std::string& uri);
    void ExtractAndSetQueryString(const std::string& uri);
    bool CompareURIParts(const URI& other) const;

    Scheme m_scheme;
    std::string m_authority;
    uint16_t m_port;
    std::string m_path;
    std::string m_queryString;
};

}

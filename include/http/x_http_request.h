//
// Created by dingrui on 1/21/26.
//

#pragma once

#include <string>
#include <string_view>
#include <regex>
#include <unordered_map>

#include "x_tcp.h"

enum class HttpMethod
{
    GET_,
    POST_,
    PUT_,
    DELETE_,
    OPTIONS_,
    UNKNOWN_
};

inline HttpMethod ParseMethod(std::string_view method)
{
    if (method == "GET") return HttpMethod::GET_;
    if (method == "POST") return HttpMethod::POST_;
    if (method == "PUT") return HttpMethod::PUT_;
    if (method == "DELETE") return HttpMethod::DELETE_;
    if (method == "OPTIONS") return HttpMethod::OPTIONS_;
    return HttpMethod::UNKNOWN_;
}

// for GET request, respond
// GET /index.html HTTP/1.1\r\n
// Host: 127.0.0.1:9527\r\n
// User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:128.0) Gecko/20100101 Firefox/128.0\r\n
// Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n
// Accept-Language: en-US,en;q=0.5\r\n
// Accept-Encoding: gzip, deflate, br, zstd\r\n
// Connection: keep-alive\r\n
// Upgrade-Insecure-Requests: 1\r\n
// Sec-Fetch-Dest: document\r\n
// Sec-Fetch-Mode: navigate\r\n
// Sec-Fetch-Site: none\r\n
// Sec-Fetch-User: ?1\r\n
// Priority: u=0, i\r\n
// \r\n
struct XHttpRequest
{
    HttpMethod                                   m_method = HttpMethod::UNKNOWN_;
    std::string                                  m_uri;
    std::string                                  m_version;
    std::unordered_map<std::string, std::string> m_headers;
    std::string                                  m_body;

    bool KeepAlive() const
    {
        auto it = m_headers.find("Connection");
        if (it == m_headers.end())
        {
            return m_version == "HTTP/1.1";
        }
        return it->second == "keep-alive";
    }
};

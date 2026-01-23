//
// Created by dingrui on 1/21/26.
//

#pragma once

#include <string>
#include <unordered_map>

#include "x_http_method.h"

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
public:
    static constexpr const char *HEADER_HOST           = "Host";
    static constexpr const char *HEADER_CONTENT_LENGTH = "Content-Length";
    static constexpr const char *HEADER_CONNECTION     = "Connection";

    HttpMethod                                   m_method  = HttpMethod::UNKNOWN_;
    std::string                                  m_uri     = "/";
    std::string                                  m_version = "HTTP/1.1";
    std::unordered_map<std::string, std::string> m_headers;
    std::string                                  m_body;

public:
    /**
     * 对请求头做前置补充
     * @param host 没配置请求头的Host话用谁来填充
     */
    void PrepareForRequest(std::string &host, uint16_t port);
    // request只负责协议 所以要提供序列话给传输用
    std::string Serialize() const;
    bool        KeepAlive() const;
    bool        HasHeader(const std::string &k) const { return this->m_headers.find(k) != this->m_headers.end(); }
    // 往header里面添加内容
    void SetHeader(std::string k, std::string v) { this->m_headers.emplace(std::move(k), std::move(v)); }
};

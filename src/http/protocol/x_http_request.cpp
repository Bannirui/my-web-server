//
// Created by dingrui on 1/21/26.
//

#include "http/protocol/x_http_request.h"

void XHttpRequest::PrepareForRequest(std::string &host, uint16_t port)
{
    // Host HTTP/1.1必须要带端口
    if (!this->HasHeader(HEADER_HOST))
    {
        this->m_headers[HEADER_HOST] = host + ":" + std::to_string(port);
    }
    // Content-Length
    if (!m_body.empty())
    {
        this->m_headers[HEADER_CONTENT_LENGTH] = std::to_string(this->m_body.size());
    }
    // Connection
    if (!this->HasHeader(HEADER_CONNECTION))
    {
        this->m_headers[HEADER_CONNECTION] = KeepAlive() ? "keep-alive" : "close";
    }
}

std::string XHttpRequest::Serialize() const
{
    std::string out;
    out.reserve(512 + this->m_body.size());
    // request line
    out.append(MethodToString(this->m_method));
    out.append(" ");
    out.append(this->m_uri);
    out.append(" ");
    out.append(this->m_version);
    out.append("\r\n");
    // headers
    for (const auto &[k, v] : m_headers)
    {
        out.append(k);
        out.append(": ");
        out.append(v);
        out.append("\r\n");
    }
    // header结束
    out.append("\r\n");
    // body
    out.append(this->m_body);
    return out;
}

bool XHttpRequest::KeepAlive() const
{
    auto it = m_headers.find(HEADER_CONNECTION);
    if (it == m_headers.end())
    {
        return m_version == "HTTP/1.1";
    }
    return it->second == "keep-alive";
}
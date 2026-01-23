//
// Created by dingrui on 1/21/26.
//

#include "http/protocol/x_http_response.h"

std::string XHttpResponse::BuildHeader() const
{
    std::string h;
    h.reserve(256);
    h.append(this->m_version);
    h.append(" ");
    h.append(std::to_string(m_status));
    h.append(" ");
    h.append(ReasonPhrase(m_status));
    h.append("\r\n");

    h.append("Server: XHTTP\r\n");

    if (!this->m_headers.count("Content-Type"))
    {
        h.append("Content-Type: text/html\r\n");
    }

    h.append("Content-Length: ");
    h.append(std::to_string(ContentLength()));
    h.append("\r\n");

    h.append("Connection: ");
    h.append(this->KeepAlive() ? "keep-alive" : "close");
    h.append("\r\n\r\n");

    return h;
}

bool XHttpResponse::KeepAlive() const
{
    auto it = this->m_headers.find("Connection");
    if (it == this->m_headers.end())
    {
        return this->m_version == "HTTP/1.1";
    }
    return it->second == "keep-alive";
}

std::string_view XHttpResponse::ReasonPhrase(int code)
{
    switch (code)
    {
        case 200:
            return "OK";
        case 400:
            return "Bad Request";
        case 403:
            return "Forbidden";
        case 404:
            return "Not Found";
        case 405:
            return "Method Not Allowed";
        case 500:
            return "Internal Server Error";
        default:
            return "Unknown";
    }
}
//
// Created by dingrui on 1/21/26.
//

#pragma once

#include <string>
#include <unordered_map>

#define CONTENT_TYPE_HTML "text/html"
#define CONTENT_TYPE_PLAIN "text/plain"

struct XHttpBody
{
    enum class Type
    {
        None,
        Memory,
        File
    };

    Type m_type = Type::None;

    // memory body
    std::string m_data;

    // file body
    int   m_fd   = -1;
    off_t m_size = 0;

    off_t Length() const
    {
        switch (m_type)
        {
            case Type::Memory:
                return m_data.size();
            case Type::File:
                return m_size;
            default:
                return 0;
        }
    }

    bool Empty() const { return m_type == Type::None; }
};

struct XHttpResp
{
    int m_status = 200;
    // client解析得到
    std::string                                  m_reason;
    std::string                                  m_version = "HTTP/1.1";
    std::unordered_map<std::string, std::string> m_headers;
    XHttpBody                                    m_body;

    // ---server专用---
    std::string BuildHeader() const
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
    // ---common api---
    off_t ContentLength() const { return this->m_body.Length(); }

    // 不维护成员表示keep-alive 从header中推导
    bool KeepAlive() const
    {
        auto it = this->m_headers.find("Connection");
        if (it == this->m_headers.end())
        {
            return this->m_version == "HTTP/1.1";
        }
        return it->second == "keep-alive";
    }

    static std::string_view ReasonPhrase(int code)
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
};

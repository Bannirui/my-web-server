//
// Created by dingrui on 1/21/26.
//

#pragma once

#include <string>

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
    int         m_status      = 200;
    std::string m_contentType = CONTENT_TYPE_HTML;
    bool        m_keepAlive   = false;

    XHttpBody m_body;

    off_t ContentLength() const { return m_body.Length(); }

    bool HasBody() const { return !m_body.Empty(); }

    std::string BuildHeader() const
    {
        std::string h;
        h.reserve(256);
        h.append("HTTP/1.1 ");
        h.append(std::to_string(m_status));
        h.append(" ");
        h.append(ReasonPhrase(m_status));
        h.append("\r\n");

        h.append("Server: XHTTP\r\n");

        if (!m_contentType.empty())
        {
            h.append("Content-Type: ");
            h.append(m_contentType);
            h.append("\r\n");
        }

        h.append("Content-Length: ");
        h.append(std::to_string(ContentLength()));
        h.append("\r\n");

        h.append("Connection: ");
        h.append(m_keepAlive ? "keep-alive" : "close");
        h.append("\r\n\r\n");

        return h;
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

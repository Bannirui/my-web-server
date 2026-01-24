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

    // memory or file. fd and size must be specified if type is file
    Type m_type = Type::None;

    // memory body or the file content
    std::string m_data;

    // file body, if type is file, fd and size both the description of that html file
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

struct XHttpResponse
{
public:
    int m_status = 200;
    // client解析得到
    std::string                                  m_reason;
    std::string                                  m_version = "HTTP/1.1";
    std::unordered_map<std::string, std::string> m_headers;
    XHttpBody                                    m_body;

public:
    // ---server专用---
    std::string BuildHeader() const;
    // ---common api---
    off_t ContentLength() const { return this->m_body.Length(); }
    // 不维护成员表示keep-alive 从header中推导
    bool                    KeepAlive() const;
    static std::string_view ReasonPhrase(int code);
};

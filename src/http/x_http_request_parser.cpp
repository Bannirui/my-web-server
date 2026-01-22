//
// Created by dingrui on 1/22/26.
//

#include "http/x_http_request_parser.h"

#include <sstream>

#include "x_tcp.h"
#include "http/x_http_request.h"
#include "x_string.h"

bool XHttpRequestParser::ReadRequest(XTcp &client, XHttpRequest &req)
{
    std::string header;
    if (!this->readHeader(client, header))
    {
        return false;
    }
    if (!this->parseHeader(header, req))
    {
        return false;
    }
    if (auto it = req.m_headers.find("Content-Length"); it != req.m_headers.end())
    {
        size_t len = std::stoul(it->second);
        if (!this->readBody(client, req.m_body, len))
        {
            return false;
        }
    }
    return true;
}

bool XHttpRequestParser::readHeader(XTcp &client, std::string &out)
{
    char buf[1024];
    while (true)
    {
        int n = client.Recv(buf, sizeof(buf));
        if (n <= 0)
        {
            return false;
        }
        buf[n] = '\0';
        out.append(buf, n);
        if (out.find("\r\n\r\n") != std::string::npos)
        {
            return true;
        }
        if (out.size() > 64 * 1024)
        {
            return false;
        }
    }
}

bool XHttpRequestParser::readBody(XTcp &client, std::string &body, size_t len)
{
    body.reserve(len);
    char buf[1024];
    while (body.size() < len)
    {
        int n = client.Recv(buf, std::min(sizeof(buf), len - body.size()));
        if (n <= 0)
        {
            return false;
        }
        body.append(buf, n);
    }
    return true;
}

bool XHttpRequestParser::parseHeader(const std::string &header, XHttpRequest &req)
{
    size_t pos = header.find("\r\n");
    if (pos == std::string::npos)
    {
        return false;
    }
    // request line
    std::istringstream iss(header.substr(0, pos));
    std::string        method;
    iss >> method >> req.m_uri >> req.m_version;
    req.m_method = ParseMethod(method);
    // headers
    size_t start = pos + 2;
    while (true)
    {
        size_t end = header.find("\r\n", start);
        if (end == start || end == std::string::npos)
        {
            break;
        }
        auto line = header.substr(start, end - start);
        if (auto colon = line.find(':'); colon != std::string::npos)
        {
            auto key = line.substr(0, colon);
            auto val = line.substr(colon + 1);
            Trim(key);
            Trim(val);
            req.m_headers[key] = val;
        }
        start = end + 2;
    }
    return true;
}

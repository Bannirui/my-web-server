//
// Created by dingrui on 1/22/26.
//

#include "http/x_http_request_parser.h"

#include <sstream>
#include <regex>

#include "x_tcp.h"
#include "http/protocol/x_http_request.h"
#include "x_string.h"
#include "http/handler/x_http_handler.h"

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
    // todo, recognize the php request
    if (req.HasHeader(XHttpRequest::HEADER_CONTENT_LENGTH))
    {
        std::string length = req.GetHeader(XHttpRequest::HEADER_CONTENT_LENGTH);
        size_t      len    = std::stoul(length);
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
    char buf[1024] = {0};
    while (body.size() < len)
    {
        int n = client.Recv(buf, std::min(sizeof(buf), len - body.size()));
        if (n <= 0)
        {
            return false;
        }
        buf[n] = '\0';
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
    // /xxx.html
    std::string uri;
    iss >> method >> uri >> req.m_version;
    req.m_method = ParseMethod(method);
    if (uri.empty() || "/" == uri)
    {
        uri.append("/").append(HTML_URI_INDEX);
    }
    this->parseUri(uri, req);
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

void XHttpRequestParser::parseUri(std::string &uri, XHttpRequest &req)
{
    if (uri.empty() || "/" == uri)
    {
        req.m_uri.m_uri = "/";
        return;
    }
    // /a.html
    std::string pattern = "^/([a-zA-Z0-9]*([.].*)?)";
    std::regex  r(pattern);
    std::smatch match;
    std::regex_search(uri, match, r);
    if (match.size() == 0)
    {
        req.m_uri.m_uri = "/";
        return;
    }
    req.m_uri.m_uri           = uri;
    req.m_uri.m_fileName      = match[1];
    std::string fileExtension = match[2];
    if (!fileExtension.empty())
    {
        // file ext is '.html', just retain 'html'
        fileExtension             = fileExtension.substr(1, fileExtension.size() - 1);
        req.m_uri.m_fileExtension = URI::ParseFileType(fileExtension);
    }
}

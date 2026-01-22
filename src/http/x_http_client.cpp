//
// Created by rui ding on 2026/1/22.
//

#include "http/x_http_client.h"

#include <sstream>

#include "http/protocol/x_http_request.h"
#include "http/protocol/x_http_resp.h"
#include "x_string.h"
#include "log/x_dump.h"
#include "log/x_log.h"

bool XHttpClient::Connect(const std::string &host, uint16_t port, int timeout_ms)
{
    this->m_host = host;
    this->m_port = port;
    if (!this->m_tcp.CreateSocket())
    {
        return false;
    }
    this->m_tcp.SetBlock(true);
    return this->m_tcp.Connect(host.c_str(), port, timeout_ms);
}

bool XHttpClient::Request(XHttpRequest &req, XHttpResp &resp)
{
    std::string remoteHost = this->m_tcp.RemoteIP();
    uint16_t    remotePort = this->m_tcp.RemotePort();
    req.PrepareForRequest(remoteHost, remotePort);
    auto raw = req.Serialize();
    XLOG_INFO("请求Request内容:{}", DumpBinary(raw.c_str(), raw.size()));
    if (!this->sendAll(raw))
    {
        return false;
    }
    return this->readResponse(resp);
}

void XHttpClient::Close()
{
    this->m_tcp.Close();
}

bool XHttpClient::sendAll(const std::string &data)
{
    size_t sent = 0;
    while (sent < data.size())
    {
        int n = this->m_tcp.Send(data.data() + sent, data.size() - sent);
        if (n <= 0)
        {
            return false;
        }
        sent += n;
    }
    return true;
}

bool XHttpClient::readResponse(XHttpResp &resp)
{
    std::string header;
    if (!this->readHeader(header))
    {
        return false;
    }
    this->parseHeader(header, resp);
    auto it = resp.m_headers.find("Content-Length");
    if (it != resp.m_headers.end())
    {
        size_t len = std::stoul(it->second);
        return this->readBody(resp.m_body, len);
    }
    return true;
}

bool XHttpClient::readHeader(std::string &out)
{
    char buf[1024] = {0};
    while (true)
    {
        int n = this->m_tcp.Recv(buf, sizeof(buf));
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

bool XHttpClient::readBody(XHttpBody &body, size_t len)
{
    body.m_type = XHttpBody::Type::Memory;
    body.m_data.reserve(len);
    char buf[1024] = {0};
    while (body.m_data.size() < len)
    {
        int n = m_tcp.Recv(buf, std::min(sizeof(buf), len - body.m_data.size()));
        if (n <= 0) return false;
        buf[0] = '\0';
        body.m_data.append(buf, n);
    }
    return true;
}

void XHttpClient::parseHeader(const std::string &header, XHttpResp &resp)
{
    std::istringstream iss(header);
    std::string        line;
    // status line
    std::getline(iss, line);
    std::istringstream status(line);
    status >> resp.m_version >> resp.m_status;
    std::getline(status, resp.m_reason);
    Trim(resp.m_reason);
    // headers
    while (std::getline(iss, line))
    {
        if (line == "\r") break;
        auto pos = line.find(':');
        if (pos == std::string::npos)
        {
            continue;
        }
        std::string k = line.substr(0, pos);
        std::string v = line.substr(pos + 1);
        Trim(k);
        Trim(v);
        resp.m_headers[k] = v;
    }
}

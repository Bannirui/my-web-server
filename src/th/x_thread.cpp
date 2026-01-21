//
// Created by dingrui on 2026/1/21.
//

#include "th/x_thread.h"

#include <string>

#include "x_log.h"

XThread::XThread(XTcp client) : m_client(std::move(client)) {}

void XThread::operator()()
{
    char buf[1024] = {0};
    // recv the client
    int recvLen = this->m_client.Recv(buf, sizeof(buf) - 1);
    if (recvLen <= 0)
    {
        this->m_client.Close();
        return;
    }
    buf[recvLen] = '\0';
    // http协议里面有\r\n转义字符 把不可见的字符转义打印日志
    std::string dump;
    dump.reserve(recvLen * 2);
    for (int i = 0; i < recvLen; ++i) {
        unsigned char c = buf[i];
        if (c == '\r') dump += "\\r";
        else if (c == '\n') dump += "\\n\n";
        else if (isprint(c)) dump += c;
        else {
            char tmp[8];
            snprintf(tmp, sizeof(tmp), "\\x%02X", c);
            dump += tmp;
        }
    }
    XLOG_INFO("recv client {} bytes:\n{}", recvLen, dump);
    // for GET request, respond
    std::string resp;
    resp.append("HTTP/1.1 200 OK\r\n");
    resp.append("Server: XHTTP\r\n");
    resp.append("Content-Type: text/html\r\n");
    resp.append("Context-Length: ").append("10\r\n");
    resp.append("\r\n");
    resp.append("0123456789");
    this->m_client.Send(resp.c_str(), resp.size() + 1);
    this->m_client.Close();
}

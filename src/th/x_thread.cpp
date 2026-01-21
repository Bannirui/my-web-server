//
// Created by dingrui on 2026/1/21.
//

#include "th/x_thread.h"

#include <string>

#include "log/x_log.h"
#include "log/x_dump.h"

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
    XLOG_INFO("recv client {} bytes:\n{}", recvLen, DumpBinary(buf, recvLen));
    // for GET request, respond
    // todo 解析请求协议中的url路径 读取对应的文件响应出去
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

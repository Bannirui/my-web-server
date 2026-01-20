//
// Created by dingrui on 2026/1/21.
//

#include "x_thread.h"

#include <string>

#include "x_log.h"
#include "x_tcp.h"

XThread::XThread(XTcp &client) : m_client(client) {}

void XThread::Main()
{
    char buf[1024] = {0};
    // recv the client
    for (;;)
    {
        int recvLen = this->m_client.Recv(buf, sizeof(buf) - 1);
        if (recvLen <= 0)
        {
            this->m_client.Close();
            delete this;
            break;
        }
        // todo rm after testing, the log cannot recognize buf
        XLOG_INFO("recv len is {}", recvLen);
        buf[recvLen] = '\0';
        XLOG_INFO("recv client {} bytes: {}", recvLen, std::string(buf, recvLen - 1));
        if (static_cast<size_t>(recvLen) < sizeof(buf) - 1)
        {
            break;
        }
    }
    // for GET request, respond
    std::string respMsg;
    respMsg.append("HTTP/1.1 200 OK\r\n");
    respMsg.append("Server: XHTTP\r\n");
    respMsg.append("Content-Type: text/html\r\n");
    respMsg.append("Context-Length: ").append("10\r\n");
    respMsg.append("\r\n");
    respMsg.append("0123456789");
    this->m_client.Send(respMsg.c_str(), respMsg.size() + 1);
    this->m_client.Close();
    delete this;
}

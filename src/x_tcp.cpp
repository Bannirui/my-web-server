//
// Created by dingrui on 1/19/26.
//

#include "x_tcp.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <arpa/inet.h>

#include "x_log.h"

#define BACK_LOG_SZ 10

int XTcp::CreateSocket()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        XLOG_ERROR("create socket failed");
    }
    this->m_sock = sock;
    return this->m_sock;
}

bool XTcp::Bind(uint16_t port)
{
    if (this->m_sock <= 0)
    {
        this->CreateSocket();
    }
    sockaddr_in s_addr;
    s_addr.sin_family      = AF_INET;
    s_addr.sin_port        = htons(port);
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (::bind(this->m_sock, (sockaddr *)&s_addr, sizeof(s_addr)) != 0)
    {
        XLOG_ERROR("{} bind to {} failed", this->m_sock, port);
        return false;
    }
    XLOG_INFO("{} bind to {} success", this->m_sock, port);
    listen(this->m_sock, BACK_LOG_SZ);
    return true;
}

XTcp XTcp::Accept()
{
    XTcp        ret;
    sockaddr_in client_addr;
    socklen_t   client_addr_len = sizeof(client_addr);
    int         client_sock     = accept(this->m_sock, (sockaddr *)&client_addr, &client_addr_len);
    if (client_sock <= 0)
    {
        return ret;
    }
    ret.m_sock = client_sock;
    ret.m_ip   = inet_ntoa(client_addr.sin_addr);
    ret.m_port = ntohs(client_addr.sin_port);
    XLOG_INFO("server accept client, client ip={} port={}, assign socket {} for communication", ret.m_ip, ret.m_port,
              ret.m_sock);
    return ret;
}

void XTcp::Close()
{
    if (this->m_sock <= 0)
    {
        return;
    }
    close(this->m_sock);
}

int XTcp::Recv(char *buf, int len)
{
    return recv(this->m_sock, buf, len, 0);
}

int XTcp::Send(const char *buf, int len)
{
    int cnt = 0;
    while (cnt != len)
    {
        int send_len = send(this->m_sock, buf + cnt, len - cnt, 0);
        if (send_len <= 0)
        {
            break;
        }
        cnt += send_len;
    }
    return cnt;
}

bool XTcp::Connect(const std::string ip, uint16_t port)
{
    if (this->m_sock <= 0)
    {
        this->CreateSocket();
    }
    sockaddr_in s_addr;
    s_addr.sin_family      = AF_INET;
    s_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    s_addr.sin_port        = htons(port);
    if (::connect(this->m_sock, (sockaddr *)&s_addr, sizeof(s_addr)) != 0)
    {
        XLOG_ERROR("{} connect to {}:{} failed, err={}", this->m_sock, ip, port, strerror(errno));
        return false;
    }
    XLOG_INFO("{} connect to {}:{} success", this->m_sock, ip, port);
    return true;
}
bool XTcp::SetBlock(bool block)
{
    if (this->m_sock <= 0)
    {
        return false;
    }
    int flag = fcntl(this->m_sock, F_GETFL, 0);
    if (flag < 0)
    {
        return false;
    }
    if (block)
    {
        flag &= ~O_NONBLOCK;
    }
    else
    {
        flag |= O_NONBLOCK;
    }
    if (fcntl(this->m_sock, F_SETFL, flag) != 0)
    {
        return false;
    }
    return true;
}

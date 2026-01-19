//
// Created by dingrui on 1/19/26.
//

#include "x_tcp.h"

#include <sys/socket.h>
#include <netinet/in.h>

#include "x_log.h"

#define BACK_LOG_SZ 10

int XTcp::CreateSocket()
{
    this->m_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (this->m_sock == -1)
    {
        XLOG_ERROR("create socket failed");
    }
    return this->m_sock;
}

bool XTcp::Bind(uint16_t port)
{
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

//
// Created by dingrui on 1/19/26.
//

#include "x_tcp.h"

#include <sys/socket.h>

#include "x_log.h"

int XTcp::CreateSocket()
{
    this->m_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (this->m_sock == -1)
    {
        XLOG_ERROR("create socket failed");
    }
    return this->m_sock;
}

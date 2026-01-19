//
// Created by dingrui on 1/19/26.
//

#include "x_tcp.h"
#include "x_log.h"

int main()
{
    XLog::Init();

    XTcp tcp;
    int  sock = tcp.CreateSocket();
    XLOG_INFO("create socket={}", sock);
    return 0;
}
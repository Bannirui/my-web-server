//
// Created by dingrui on 1/19/26.
//

#include "log/x_log.h"
#include "x_tcp.h"
#include "log/x_dump.h"

int main(int argc, char *argv[])
{
    XLog::Init();

    XTcp client;
    client.CreateSocket();
    client.SetBlock(false);
    client.Connect("127.0.0.1", 9527, 5000);
    client.Send("client", 7);
    char buf[1024] = {0};
    int  recvLen   = client.Recv(buf, sizeof(buf) - 1);
    buf[recvLen]   = '\0';
    XLOG_INFO("recv {} bytes:{}", recvLen, DumpBinary(buf, recvLen));
    client.Close();
    return 0;
}

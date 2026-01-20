//
// Created by dingrui on 1/19/26.
//

#include "x_log.h"
#include "x_tcp.h"

int main(int argc, char *argv[])
{
    XLog::Init();

    XTcp client;
    client.CreateSocket();
    client.SetBlock(false);
    client.Connect("127.0.0.1", 9527);
    client.Send("client", 7);
    char buf[1024] = {0};
    int  recv_len  = client.Recv(buf, sizeof(buf) - 1);
    XLOG_INFO("recv: {}", std::string(buf, recv_len - 1));
    client.Close();
    return 0;
}

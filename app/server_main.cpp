//
// Created by dingrui on 1/19/26.
//

#include "x_log.h"
#include "x_tcp.h"

#include <sys/socket.h>
#include <thread>
#include <string>

class TcpThread
{
public:
    TcpThread(XTcp client) : m_client(client) {}

    void Main()
    {
        char buf[1024];
        for (;;)
        {
            int recv_len = this->m_client.Recv(buf, sizeof(buf) - 1);
            if (recv_len <= 0)
            {
                break;
            }
            buf[recv_len] = '\0';
            XLOG_INFO("recv {} bytes, {}", recv_len, std::string(buf, recv_len - 1));
            if (strstr(buf, "quit") != nullptr)
            {
                char re[] = "quit success\n";
                this->m_client.Send(re, strlen(re) + 1);
                break;
            }
            int send_len = this->m_client.Send("ok\n", 4);
            XLOG_INFO("send {} bytes", send_len);
            this->m_client.Close();
            delete this;
        }
    }

private:
    XTcp m_client;
};

int main(int argc, char *argv[])
{
    XLog::Init();

    uint16_t port = 9527;
    if (argc > 1)
    {
        port = atoi(argv[1]);
    }
    XTcp server;
    server.CreateSocket();
    server.Bind(port);
    for (;;)
    {
        XTcp        client = server.Accept();
        TcpThread  *th     = new TcpThread(client);
        std::thread sth(&TcpThread::Main, th);
        sth.detach();
    }
    server.Close();
    return 0;
}
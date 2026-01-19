//
// Created by dingrui on 1/19/26.
//

#include "x_tcp.h"
#include "x_log.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

int main()
{
    XLog::Init();

    std::atomic<bool> running{true};

    uint16_t    port        = 9527;
    int         server_sock = 0;
    std::thread server_thread(
        [&](void)
        {
            XTcp tcp;
            server_sock = tcp.CreateSocket();
            XLOG_INFO("create socket={}", server_sock);
            tcp.Bind(port);
            XTcp client = tcp.Accept();
            running = false;
        });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::thread client_thread(
        [&](void)
        {
            int client_sock = socket(AF_INET, SOCK_STREAM, 0);
            if (client_sock <= 0)
            {
                perror("socket");
                return;
            }
            sockaddr_in addr{};
            addr.sin_family = AF_INET;
            addr.sin_port   = htons(port);
            inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
            int ret = ::connect(client_sock, reinterpret_cast<sockaddr *>(&addr), sizeof(addr));
            if (ret < 0)
            {
                perror("connect");
            }
            XLOG_INFO("client connected, sock={}", client_sock);
            ::close(client_sock);
        });

    client_thread.join();
    server_thread.join();
    return 0;
}
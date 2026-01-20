//
// Created by dingrui on 1/19/26.
//

#include <sys/epoll.h>

#include "x_log.h"
#include "x_tcp.h"
#include "x_thread.h"

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
    int         ep_fd = epoll_create(256);
    epoll_event ev{};
    ev.data.fd = server.get_sock();
    // EPOLLIN=connect and close
    ev.events = EPOLLIN | EPOLLET;
    epoll_ctl(ep_fd, EPOLL_CTL_ADD, ev.data.fd, &ev);
    epoll_event events[1024];
    // char        buf[1024] = {0};
    // const char *msg       = "HTTP/1.1 200 OK\r\nContent-Length: 1\r\nX\r\n\r\n";
    server.SetBlock(false);
    for (;;)
    {
        int cnt = epoll_wait(ep_fd, events, 1024, 500);
        if (cnt <= 0)
        {
            continue;
        }
        for (int i = 0; i < cnt; ++i)
        {
            // connect or read
            if (events[i].data.fd == server.get_sock())
            {
                // clone a new socket, and register to selector
                for (;;)
                {
                    // selector is Edge Mode, it maybe multiple connects at the same moment
                    // however, the accept call maybe blocked, so set the socket as non-blocking
                    // and then stopping accept since the accepting socket is invalid
                    XTcp client = server.Accept();
                    if (client.get_sock() <= 0)
                    {
                        break;
                    }
                    ev.data.fd = client.get_sock();
                    ev.events  = EPOLLIN | EPOLLET;
                    epoll_ctl(ep_fd, EPOLL_CTL_ADD, ev.data.fd, &ev);
                }
            }
            else
            {
                // data arrive in, we can read data from according socket
                // XTcp client;
                // client.set_sock(events[i].data.fd);
                // client.Recv(buf, sizeof(buf) - 1);
                // client.Send(msg, strlen(msg) + 1);
                // // remove from selector
                // epoll_ctl(ep_fd, EPOLL_CTL_DEL, client.get_sock(), &ev);
                // client.Close();

                XTcp client;
                client.set_sock(events[i].data.fd);
                XThread    *th = new XThread(client);
                std::thread sth(&XThread::Main, th);
                sth.detach();
            }
        }
        // TcpThread  *th = new TcpThread(client);
        // std::thread sth(&TcpThread::Main, th);
        // sth.detach();
    }
    server.Close();
    return 0;
}
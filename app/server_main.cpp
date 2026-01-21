//
// Created by dingrui on 1/19/26.
//

#include "log/x_log.h"
#include "x_tcp.h"
#include "th/x_thread.h"
#include "th/x_thread_pool.h"
#include "x_poll_factory.h"

// 线程池
XThreadPool pool(std::thread::hardware_concurrency());

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
    server.SetBlock(false);

    XPoller *poller = CreatePoller();
    poller->Init();
    poller->Add(server.get_sock());

    // 收多路复用器里面就绪的socket
    std::vector<XEvent> events;
    for (;;)
    {
        events.clear();
        int n = poller->Wait(events, 500);
        if (n <= 0)
        {
            continue;
        }
        for (auto &ev : events)
        {
            // connect or read
            if (ev.fd == server.get_sock())
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
                    poller->Add(client.get_sock());
                }
            }
            else
            {
                // data arrive in, we can read data from according socket
                XTcp client;
                client.set_sock(ev.fd);
                // 从系统多路复用器删除防止重复触发
                poller->Del(ev.fd);
                // 封装成任务提交给线程池
                pool.Submit(XThread(client));
            }
        }
    }
    delete poller;
    server.Close();
    return 0;
}
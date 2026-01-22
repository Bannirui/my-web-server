//
// Created by dingrui on 1/19/26.
//

#include "log/x_log.h"
#include "x_tcp.h"
#include "th/x_worker.h"
#include "th/x_thread_pool.h"
#include "x_poll_factory.h"
#include "http/handler/x_delete_handler.h"
#include "http/handler/x_get_handler.h"
#include "http/x_http_dispatcher.h"
#include "http/protocol/x_http_request.h"
#include "http/handler/x_options_handler.h"
#include "http/handler/x_post_handler.h"
#include "http/handler/x_put_handler.h"

// 线程池
XThreadPool pool(std::thread::hardware_concurrency());

int main(int argc, char *argv[])
{
    XLog::Init();

    auto dispatcher = std::make_shared<XHttpDispatcher>();
    dispatcher->Register(HttpMethod::GET_, std::make_unique<XGetHandler>());
    dispatcher->Register(HttpMethod::POST_, std::make_unique<XPostHandler>());
    dispatcher->Register(HttpMethod::PUT_, std::make_unique<XPutHandler>());
    dispatcher->Register(HttpMethod::DELETE_, std::make_unique<XDeleteHandler>());
    dispatcher->Register(HttpMethod::OPTIONS_, std::make_unique<XOptionsHandler>());

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
                pool.Submit(XWorker(client, dispatcher));
            }
        }
    }
    delete poller;
    server.Close();
    return 0;
}
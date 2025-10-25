//
// Created by rui ding on 2025/9/11.
//
#pragma once

#include "connection_pool.h"

#include "event_loop.h"
#include "socket.h"
#include "thread_pool.h"

#define LISTEN_PORT 9527
#define THREAD_POOL_THREADS 32

namespace my_ws
{
    class WebServer
    {
    public:
        explicit WebServer(uint16_t port = LISTEN_PORT);
        ~WebServer();
        void Run();

    private:
        /**
         * server socket is registered to selector, some client connect, and selector works
         */
        void onAccept() const;

    private:
        Socket                          _listenSock;
        std::unique_ptr<EventLoop>      _eventLoop;
        ThreadPool                      _threadPool;
        std::unique_ptr<ConnectionPool> _connPool;
    };
}  // namespace my_ws
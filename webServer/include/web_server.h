//
// Created by rui ding on 2025/9/11.
//
#pragma once

#include "event_loop.h"
#include "socket.h"
#include "thread_pool.h"

namespace my_ws
{
    class WebServer
    {
    public:
        WebServer();
        ~WebServer();
        void run();

    private:
    };
}  // namespace my_ws
//
// Created by dingrui on 2025/10/23.
//
#pragma once

#include <chrono>
#include <memory>

#include "event_loop.h"
#include "http_connect.h"

#define CONN_IDLE_TIMEOUT_SEC 30

namespace my_ws
{
    class ConnectionPool
    {
    public:
        explicit ConnectionPool(EventLoop &eventLoop, int idleTimeoutSec = CONN_IDLE_TIMEOUT_SEC);
        ~ConnectionPool();

        void Add(int fd);
        void Remove(int fd);
        void RefreshActive(int fd);
        void StartTimer();

    private:
        struct ConnInfo
        {
            std::unique_ptr<HttpConnect>          conn;
            std::chrono::steady_clock::time_point lastActive;
        };
        void closeIdleConns();

    private:
        EventLoop                        &_eventLoop;
        std::unordered_map<int, ConnInfo> _conns;

        int _timerFd{-1};
        int _idleTimeoutSec{30};
    };
}  // namespace my_ws
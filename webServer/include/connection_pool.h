//
// Created by dingrui on 2025/10/23.
//
#pragma once

#include <chrono>
#include <memory>
#include <map>

#include "event_loop.h"
#include "http_connect.h"

#define CONN_POOL_CAP 4096
#define CONN_IDLE_TIMEOUT_SEC 60

namespace my_ws
{
    class ConnectionPool
    {
    public:
        explicit ConnectionPool(EventLoop &eventLoop, int maxSz = CONN_POOL_CAP,
                                int idleTimeoutSec = CONN_IDLE_TIMEOUT_SEC);
        ~ConnectionPool();

        int  GetCurConnCnt() const { return _connMaxCnt; };
        int  GetConnMaxCnt() const { return _conns.size(); };
        void Add(int fd);
        void Remove(int fd);
        void RefreshActive(int fd);
        void StartTimer();

    private:
        struct ConnInfo
        {
            std::unique_ptr<HttpConnect>                                        conn;
            std::chrono::steady_clock::time_point                               lastActive;
            std::multimap<std::chrono::steady_clock::time_point, int>::iterator byActiveIter;
        };
        void closeIdleConns();

    private:
        EventLoop &_eventLoop;
        // key is fd
        std::unordered_map<int, ConnInfo> _conns;
        // key is last active time, val is fd
        std::multimap<std::chrono::steady_clock::time_point, int> _connsAscByActiveTime;

        int _timerFd{-1};
        int _idleTimeoutSec{0};
        int _connMaxCnt{0};
    };
}  // namespace my_ws
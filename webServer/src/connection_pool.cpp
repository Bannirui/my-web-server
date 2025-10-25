//
// Created by dingrui on 2025/10/23.
//

#include "connection_pool.h"

#include "log.h"

#include <unistd.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>

my_ws::ConnectionPool::ConnectionPool(EventLoop &eventLoop, int maxSz, int idleTimeoutSec)
    : _eventLoop(eventLoop), _idleTimeoutSec(idleTimeoutSec), _connMaxCnt(maxSz)
{
}

my_ws::ConnectionPool::~ConnectionPool()
{
    if (_timerFd >= 0)
    {
        _eventLoop.DelFd(_timerFd);
        close(_timerFd);
    }
    for (auto &p : _conns)
    {
        int fd = p.first;
        _eventLoop.DelFd(fd);
    }
    _conns.clear();
}

void my_ws::ConnectionPool::Add(int fd)
{
    auto conn = std::make_unique<HttpConnect>(fd, _eventLoop);
    auto now  = std::chrono::steady_clock::now();
    auto iter = _connsAscByActiveTime.emplace(now, fd);
    // cache
    _conns.emplace(fd, ConnInfo{std::move(conn), now, iter});
    // register to selector
    _eventLoop.AddFd(fd, EPOLLIN | EPOLLET,
                     [this, fd](uint32_t ev)
                     {
                         auto entry = _conns.find(fd);
                         if (entry == _conns.end())
                         {
                             // no connection found
                             _eventLoop.DelFd(fd);
                             return;
                         }
                         auto &connInfo = entry->second;
                         auto  now      = std::chrono::steady_clock::now();
                         if (ev & EPOLLIN)
                         {
                             connInfo.conn->OnRead();
                             connInfo.lastActive = now;
                         }
                         if (ev & EPOLLOUT)
                         {
                             connInfo.conn->OnWrite();
                             connInfo.lastActive = now;
                         }
                         if (ev & (EPOLLHUP | EPOLLERR))
                         {
                             LOG_INFO("epoll HUP/ERR on fd={}, remove connection", fd);
                             _eventLoop.DelFd(fd);
                             _conns.erase(entry);
                             return;
                         }
                         // clean
                         if (connInfo.conn->Fd() < 0)
                         {
                             LOG_INFO("connection fd {} closed by peer/handler, remove", fd);
                             _eventLoop.DelFd(fd);
                             _conns.erase(entry);
                         }
                     });
    LOG_INFO("Added connection fd={} to pool", fd);
}

void my_ws::ConnectionPool::Remove(int fd)
{
    auto it = _conns.find(fd);
    if (it == _conns.end())
    {
        return;
    }
    _eventLoop.DelFd(fd);
    _conns.erase(it);
    _connsAscByActiveTime.erase(it->second.byActiveIter);
    LOG_INFO("remove connection fd={} from pool", fd);
}

void my_ws::ConnectionPool::RefreshActive(int fd)
{
    auto now = std::chrono::steady_clock::now();
    auto it  = _conns.find(fd);
    if (it != _conns.end())
    {
        _connsAscByActiveTime.erase(it->second.byActiveIter);
        it->second.lastActive   = now;
        it->second.byActiveIter = _connsAscByActiveTime.emplace(now, fd);
    }
}

void my_ws::ConnectionPool::StartTimer()
{
    _timerFd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if (_timerFd < 0)
    {
        LOG_ERROR("timerfd_create failed, {}", strerror(errno));
        throw std::runtime_error("failed to create timerfd");
    }

    itimerspec spec{};
    spec.it_value.tv_sec    = _idleTimeoutSec;
    spec.it_interval.tv_sec = _idleTimeoutSec;
    timerfd_settime(_timerFd, 0, &spec, nullptr);
    _eventLoop.AddFd(_timerFd, EPOLLIN,
                     [this](uint32_t events)
                     {
                         (void)events;
                         closeIdleConns();
                     });
}

void my_ws::ConnectionPool::closeIdleConns()
{
    auto now        = std::chrono::steady_clock::now();
    auto expireTime = now - std::chrono::seconds(_idleTimeoutSec);
    for (auto it = _connsAscByActiveTime.begin(); it != _connsAscByActiveTime.end();)
    {
        if (it->first > expireTime) break;  // the earliest is not expired, all later ones are active enough
        int fd = it->second;
        LOG_INFO("Closing idle connection {}", fd);
        it = _connsAscByActiveTime.erase(it);  // advance safety
        _conns.erase(fd);
    }
}

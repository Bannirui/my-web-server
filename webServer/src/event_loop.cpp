#include <sys/epoll.h>
#include <unistd.h>

#include "event_loop.h"
#include "log.h"

#include <iostream>

my_ws::EventLoop::EventLoop()
{
    _selectorId = epoll_create1(0);
    if (_selectorId < 0)
    {
        throw std::runtime_error("epoll_create1");
    }
    LOG_INFO("selector created, fd={}", _selectorId);
}

my_ws::EventLoop::~EventLoop()
{
    if (_selectorId >= 0)
    {
        close(_selectorId);
    }
}

void my_ws::EventLoop::Run()
{
    const int                MAXE = 1024;
    std::vector<epoll_event> evs(MAXE);
    _running = true;
    while (_running)
    {
        // fix timeout
        int n = epoll_wait(_selectorId, evs.data(), MAXE, -1);
        if (n < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            perror("epoll_wait");
            break;
        }
        LOG_INFO("selector got {} ready events", n);
        for (int i = 0; i < n; ++i)
        {
            int      fd     = evs[i].data.fd;
            uint32_t events = evs[i].events;
            auto     it     = _handlers.find(fd);
            if (it != _handlers.end())
            {
                LOG_INFO("find {} for ready event", fd);
                try
                {
                    it->second.cb(events);
                }
                catch (std::exception &ex)
                {
                    LOG_ERROR("handler exception: {}", ex.what());
                }
                break;
            }
        }
    }
}

void my_ws::EventLoop::Stop()
{
    _running = false;
}

void my_ws::EventLoop::AddFd(int fd, u_int32_t events, FdCallback cb)
{
    _handlers.emplace(fd, Entry{fd, events, std::move(cb)});
    epoll_event ev{};
    ev.events  = events;
    ev.data.fd = fd;
    if (epoll_ctl(_selectorId, EPOLL_CTL_ADD, fd, &ev) < 0)
    {
        LOG_ERROR("add fd {} to event loop failed, {}", fd, strerror(errno));
        throw std::runtime_error("epoll_ctl");
    }
    LOG_INFO("add fd {} to event loop", fd);
}

void my_ws::EventLoop::ModifyFd(int fd, u_int32_t events)
{
    epoll_event ev;
    ev.events  = events;
    ev.data.fd = fd;
    if (epoll_ctl(_selectorId, EPOLL_CTL_MOD, fd, &ev) < 0)
    {
        throw std::runtime_error("epoll_ctl");
    }
    auto it = _handlers.find(fd);
    if (it != _handlers.end())
    {
        it->second.events = events;
    }
}

void my_ws::EventLoop::DelFd(int fd)
{
    epoll_ctl(_selectorId, EPOLL_CTL_DEL, fd, NULL);
    _handlers.erase(fd);
    LOG_INFO("delete fd {} from event loop", fd);
}
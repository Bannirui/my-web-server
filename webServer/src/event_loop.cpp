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
    const int                MAXE = 64;
    std::vector<epoll_event> evs(MAXE);
    _running = true;
    while (_running)
    {
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
            for (auto &h : _handlers)
            {
                if (h.fd == fd)
                {
                    try
                    {
                        h.cb(events);
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
}

void my_ws::EventLoop::Stop()
{
    _running = false;
}

void my_ws::EventLoop::AddFd(int fd, u_int32_t events, FdCallback cb)
{
    LOG_INFO("add fd {} to event loop", fd);
    epoll_event ev;
    ev.events  = events;
    ev.data.fd = fd;
    if (epoll_ctl(_selectorId, EPOLL_CTL_ADD, fd, &ev) < 0)
    {
        throw std::runtime_error("epoll_ctl");
    }
    _handlers.push_back({fd, events, cb});
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
    for (auto &e : _handlers)
    {
        if (e.fd == fd)
        {
            e.events = events;
        }
    }
}

void my_ws::EventLoop::DelFd(int fd)
{
    epoll_ctl(_selectorId, EPOLL_CTL_DEL, fd, NULL);
    _handlers.erase(std::remove_if(_handlers.begin(), _handlers.end(), [fd](const Entry &e) { return e.fd == fd; }),
                    _handlers.end());
}
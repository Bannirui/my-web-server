//
// Created by rui ding on 2026/1/21.
//

#ifdef __linux__

#include "ev/x_epoll_poller.h"

#include <sys/epoll.h>
#include <unistd.h>

bool XEpollPoller::Init()
{
    this->m_ep_fd = epoll_create(256);
    return this->m_ep_fd > 0;
}

bool XEpollPoller::Add(int fd)
{
    epoll_event ev{};
    ev.data.fd = fd;
    // 监听EPOLLIN 会触发的时机包括 客户端connect的时候 客户端send的时候 客户端close的时候也会触发
    ev.events  = EPOLLIN | EPOLLET;
    return epoll_ctl(this->m_ep_fd, EPOLL_CTL_ADD, fd, &ev) == 0;
}

bool XEpollPoller::Del(int fd)
{
    epoll_event ev{};
    ev.data.fd = fd;
    ev.events  = EPOLLIN | EPOLLET;
    // 最后一个参数理论上nullptr就行 但是在实践中发现NULL可能会报错 就构建个数据传进去
    return epoll_ctl(this->m_ep_fd, EPOLL_CTL_DEL, fd, &ev) == 0;
}

int XEpollPoller::Wait(std::vector<XEvent> &events, int timeout_ms)
{
    epoll_event evs[1024];
    int         n = epoll_wait(this->m_ep_fd, evs, 1024, timeout_ms);
    for (int i = 0; i < n; ++i)
    {
        events.push_back({evs[i].data.fd});
    }
    return n;
}

#endif
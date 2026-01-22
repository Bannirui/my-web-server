//
// Created by rui ding on 2026/1/21.
//

#ifdef __APPLE__

#include "ev/x_kqueue_poller.h"

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

bool XKqueuePoller::Init()
{
    this->m_kq_fd = kqueue();
    if (this->m_kq_fd <= 0)
    {
        return false;
    }
    struct kevent ev{};
    EV_SET(&ev, WAKEUP_IDENT, EVFILT_USER, EV_ADD | EV_CLEAR, 0, 0, nullptr);
    // 事件注册到kq
    return kevent(m_kq_fd, &ev, 1, nullptr, 0, nullptr) == 0;
}

bool XKqueuePoller::Add(int fd)
{
    struct kevent ev;
    EV_SET(&ev, fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, nullptr);
    return kevent(this->m_kq_fd, &ev, 1, nullptr, 0, nullptr) == 0;
}

bool XKqueuePoller::Del(int fd)
{
    struct kevent ev;
    EV_SET(&ev, fd, EVFILT_READ, EV_DELETE, 0, 0, nullptr);
    return kevent(this->m_kq_fd, &ev, 1, nullptr, 0, nullptr) == 0;
}

int XKqueuePoller::Wait(std::vector<XEvent> &events, int timeout_ms)
{
    struct kevent   evs[1024];
    struct timespec ts;
    ts.tv_sec  = timeout_ms / 1000;
    ts.tv_nsec = (timeout_ms % 1000) * 1000000;

    int n = kevent(this->m_kq_fd, nullptr, 0, evs, 1024, &ts);
    if (n <= 0)
    {
        return n;
    }
    for (int i = 0; i < n; ++i)
    {
        // 我自己注册的事件 这个时候wait已经被唤醒执行到这个 这个事件丢掉不要给client和server
        if (evs[i].filter == EVFILT_USER && evs[i].ident == WAKEUP_IDENT)
        {
            continue;
        }
        events.push_back({static_cast<int>(evs[i].ident)});
    }
    return static_cast<int>(events.size());
}

void XKqueuePoller::Wakeup()
{
    // 在kq初始化时候注册了这个事件 现在触发它 实现对wait的打断
    struct kevent ev;
    EV_SET(&ev, WAKEUP_IDENT, EVFILT_USER, 0, NOTE_TRIGGER, 0, nullptr);
    kevent(m_kq_fd, &ev, 1, nullptr, 0, nullptr);
}

#endif
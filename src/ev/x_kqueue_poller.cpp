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
    return this->m_kq_fd > 0;
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
    struct kevent evs[1024];

    struct timespec ts;
    ts.tv_sec  = timeout_ms / 1000;
    ts.tv_nsec = (timeout_ms % 1000) * 1000000;

    int n = kevent(this->m_kq_fd, nullptr, 0, evs, 1024, &ts);
    for (int i = 0; i < n; ++i)
    {
        events.push_back({static_cast<int>(evs[i].ident)});
    }
    return n;
}

#endif
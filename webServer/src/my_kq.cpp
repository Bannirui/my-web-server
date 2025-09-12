//
// Created by rui ding on 2025/9/12.
//

#include <sys/event.h>

#include "my_kq.h"
#include "log/log.h"

MyKqueue::MyKqueue(TriggerMode mode)
    : m_kq(kqueue(),
           [](int fd)
           {
               if (fd != -1)
               {
                   if (close(fd) == -1)
                   {
                       perror("close");
                   }
                   else
                   {
                       MY_LOG_INFO("关闭kqueue{}", fd);
                   }
               }
           }),
      m_mode(mode)
{
    if (m_kq.get() == -1)
    {
        throw std::runtime_error("kqueue创建失败");
    }
}
void MyKqueue::addReadEvent(int fd) const
{
    struct kevent ev{};
    EV_SET(&ev, fd, EVFILT_READ, EV_ADD | getFlags(), 0, 0, nullptr);
    if (kevent(m_kq.get(), &ev, 1, nullptr, 0, nullptr) == -1)
    {
        MY_LOG_ERROR("向kqueue{}添加读事件{}失败", this->getFd(), fd);
        throw std::runtime_error("向kqueue添加读事件失败");
    }
}

void MyKqueue::addWriteEvent(int fd) const
{
    struct kevent ev{};
    EV_SET(&ev, fd, EVFILT_WRITE, EV_ADD | getFlags(), 0, 0, nullptr);
    if (kevent(m_kq.get(), &ev, 1, nullptr, 0, nullptr) == -1)
    {
        MY_LOG_ERROR("向kqueue{}添加写事件{}失败", this->getFd(), fd);
        throw std::runtime_error("向kqueue添加写事件失败");
    }
}

void MyKqueue::removeEvent(int fd, int filter) const
{
    struct kevent ev{};
    EV_SET(&ev, fd, filter, EV_DELETE, 0, 0, nullptr);
    // 即使删除失败 比如fd已关闭 这里不抛异常 避免影响流程
    kevent(m_kq.get(), &ev, 1, nullptr, 0, nullptr);
}

std::vector<struct kevent> MyKqueue::wait(int timeoutMs) const
{
    std::vector<struct kevent> events(64);
    struct timespec            ts;
    struct timespec*           pts = nullptr;
    if (timeoutMs >= 0)
    {
        ts.tv_sec  = timeoutMs / 1000;
        ts.tv_nsec = (timeoutMs % 1000) * 1000000;
        pts        = &ts;
    }
    int n = ::kevent(this->m_kq.get(), nullptr, 0, events.data(), static_cast<int>(events.size()), pts);
    if (n < 0)
    {
        if (errno == EINTR) return {};
        MY_LOG_ERROR("kqueue{}等待就绪事件失败", this->getFd());
        throw std::runtime_error("kqueue等待就绪事件失败");
    }
    events.resize(n);
    return events;
}
int MyKqueue::getFlags() const
{
    return (m_mode == TriggerMode::EDGE) ? EV_CLEAR : 0;
}

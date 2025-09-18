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
    MY_LOG_INFO("kqueue{}创建成功", this->m_kq.get());
}
void MyKqueue::addReadEvent(int fd, bool oneShot) const
{
    struct kevent ev{};
    int           flags = EV_ADD;
    // 边缘触发 水平触发
    flags |= this->getFlags();
    // 一次性事件
    if (oneShot) flags |= EV_ONESHOT;
    EV_SET(&ev, fd, EVFILT_READ, flags, 0, 0, nullptr);
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
void MyKqueue::addSignal(int sig) const
{
    // 注册信号 让信号进入kqueue
    struct kevent ev{};
    EV_SET(&ev, sig, EVFILT_SIGNAL, EV_ADD, 0, 0, NULL);
    assert(kevent(this->getFd(), &ev, 1, nullptr, 0, nullptr) != -1);
    // 交给kqueue 不让默认处理 确保不会被默认终止/忽略
    signal(sig, SIG_IGN);
}

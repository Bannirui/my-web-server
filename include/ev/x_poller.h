//
// Created by rui ding on 2026/1/21.
//

#pragma once

#include <vector>

struct XEvent
{
    int fd;
};

// 系统多路复用器的抽象 屏蔽平台的epoll或kqueue
class XPoller
{
public:
    virtual ~XPoller() = default;

    virtual bool Init()      = 0;
    virtual bool Add(int fd) = 0;
    virtual bool Del(int fd) = 0;

    /**
     * @param timeout_ms 阻塞式带超时 单位毫秒
     * @return 就绪的事件个数
     */
    virtual int Wait(std::vector<XEvent> &events, int timeout_ms) = 0;

protected:
    XPoller() = default;
};

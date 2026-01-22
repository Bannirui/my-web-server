//
// Created by rui ding on 2026/1/21.
//

#pragma once
#ifdef __APPLE__

#include "x_poller.h"

class XKqueuePoller : public XPoller
{
public:
    /**
     * kq有用户自定义事件
     * 在初始化的时候注册一个事件 调用Wakeup时候触发它就实现了主动打断wait
     */
    bool Init() override;
    bool Add(int fd) override;
    bool Del(int fd) override;
    int  Wait(std::vector<XEvent> &events, int timeout_ms) override;
    void Wakeup() override;

private:
    int m_kq_fd{-1};
    // 通过kq的用户自定义事件实现wakeup唤醒 自定义id
    static constexpr uintptr_t WAKEUP_IDENT = 1;
};

#endif

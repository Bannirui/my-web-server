//
// Created by rui ding on 2026/1/21.
//

#pragma once
#ifdef __linux__

#include "x_poller.h"

class XEpollPoller : public XPoller
{
public:
    bool Init() override;
    bool Add(int fd) override;
    bool Del(int fd) override;
    int  Wait(std::vector<XEvent> &events, int timeout_ms) override;
    void Wakeup() override;

private:
    int m_ep_fd{-1};
    // 用来实现打断wait实现wakeup唤醒
    int m_wakeup_fd{-1};
};

#endif

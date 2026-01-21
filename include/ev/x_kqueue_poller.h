//
// Created by rui ding on 2026/1/21.
//

#pragma once
#ifdef __APPLE__

#include "x_poller.h"

class XKqueuePoller : public XPoller
{
public:
    bool Init() override;
    bool Add(int fd) override;
    bool Del(int fd) override;
    int  Wait(std::vector<XEvent> &events, int timeout_ms) override;

private:
    int m_kq_fd{-1};
};

#endif

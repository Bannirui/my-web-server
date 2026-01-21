//
// Created by rui ding on 2026/1/21.
//

#pragma once

#include "ev/x_poller.h"

#ifdef __APPLE__
#include "ev/x_kqueue_poller.h"
inline XPoller* CreatePoller() { return new XKqueuePoller(); }
#else
#include "ev/x_epoll_poller.h"
inline XPoller* CreatePoller() { return new XEpollPoller(); }
#endif

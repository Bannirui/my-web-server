//
// Created by dingrui on 2026/1/21.
//
#pragma once

#include <memory>

#include "x_tcp.h"

class XThread
{
public:
    explicit XThread(XTcp client);

    // 执行任务
    void operator()();

private:
    XTcp m_client;
};
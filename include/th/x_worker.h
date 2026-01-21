//
// Created by dingrui on 2026/1/21.
//
#pragma once

#include <memory>

#include "x_tcp.h"
#include "http/x_http_dispatcher.h"

class XWorker
{
public:
    explicit XWorker(XTcp client, std::shared_ptr<const XHttpDispatcher> dispatcher);

    // 执行任务
    void operator()();

private:
    XTcp                                   m_client;
    std::shared_ptr<const XHttpDispatcher> m_dispatcher;
};
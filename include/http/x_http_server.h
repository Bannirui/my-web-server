//
// Created by rui ding on 2026/1/22.
//

#pragma once

#include <thread>

#include "x_http_dispatcher.h"
#include "x_tcp.h"
#include "ev/x_poller.h"
#include "th/x_thread_pool.h"

class XHttpServer
{
public:
    /**
     * @param thread_num 线程池大小默认cpu核数
     */
    explicit XHttpServer(uint16_t port, size_t thread_num = std::thread::hardware_concurrency());

    ~XHttpServer();

    void SetDispatcher(const std::shared_ptr<XHttpDispatcher> &dispatcher) { this->m_dispatcher = dispatcher; };

    bool Init();
    void Run();
    void Stop();

private:
    // 服务端收到客户端的连接进来 accept系统调用会在服务端克隆新的socket负责跟客户端进行读写通信
    // 把这个新socket注册到复用器等可读
    void handleAccept();
    // 客户端有数据进来 从socket读出来
    void handleClient(int fd);

private:
    // 当前服务监听在哪个端口上
    uint16_t m_port;
    // Tcp连接用来accept客户端的连接
    XTcp m_listen;
    // server状态
    std::atomic<bool> m_running{false};

    std::unique_ptr<XPoller> m_poller;
    XThreadPool              m_pool;

    std::shared_ptr<XHttpDispatcher> m_dispatcher;
    // 收多路复用器里面就绪的socket
    std::vector<XEvent> m_events;
};

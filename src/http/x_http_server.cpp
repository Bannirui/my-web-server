//
// Created by rui ding on 2026/1/22.
//

#include "http/x_http_server.h"

#include "x_poll_factory.h"
#include "th/x_worker.h"

XHttpServer::XHttpServer(uint16_t port, size_t thread_num) : m_port(port), m_pool(thread_num) {}

XHttpServer::~XHttpServer()
{
    this->Stop();
}

bool XHttpServer::Init()
{
    if (!this->m_listen.CreateSocket())
    {
        return false;
    }
    if (!this->m_listen.Bind(this->m_port))
    {
        return false;
    }
    this->m_listen.SetBlock(false);
    this->m_poller.reset(CreatePoller());
    if (!this->m_poller || !this->m_poller->Init())
    {
        return false;
    }
    this->m_poller->Add(this->m_listen.get_sock());
    return true;
}

void XHttpServer::Run()
{
    this->m_running.store(true);
    while (this->m_running.load())
    {
        this->m_events.clear();
        // 就绪的socket
        int n = this->m_poller->Wait(this->m_events, 500);
        if (n <= 0)
        {
            continue;
        }
        for (auto &ev : m_events)
        {
            // connect or read
            if (ev.fd == this->m_listen.get_sock())
            {
                handleAccept();
            }
            else
            {
                handleClient(ev.fd);
            }
        }
    }
}

void XHttpServer::Stop()
{
    if (!this->m_running.exchange(false))
    {
        // already stopped
        return;
    }
    // 先关闭监听socket 防止新连接
    this->m_listen.Close();
    // 让poller立即返回
    if (this->m_poller)
    {
        this->m_poller->Wakeup();
    }
    // 等线程池里的任务执行完
    this->m_pool.Shutdown();
}

void XHttpServer::handleAccept()
{
    // clone a new socket, and register to selector
    for (;;)
    {
        // selector is Edge Mode, it maybe multiple connects at the same moment
        // however, the accept call maybe blocked, so set the socket as non-blocking
        // and then stopping accept since the accepting socket is invalid
        XTcp client = this->m_listen.Accept();
        if (client.get_sock() <= 0)
        {
            break;
        }
        // 客户端新来的连接注册到selector上 以后进行读写
        this->m_poller->Add(client.get_sock());
    }
}

void XHttpServer::handleClient(int fd)
{
    // data arrive in, we can read data from according socket
    XTcp client;
    client.set_sock(fd);
    // 从系统多路复用器删除防止重复触发
    this->m_poller->Del(fd);
    // 封装成任务提交给线程池
    this->m_pool.Submit(XWorker(std::move(client), this->m_dispatcher));
}

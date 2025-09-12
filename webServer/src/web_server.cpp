//
// Created by rui ding on 2025/9/11.
//

#include <sys/socket.h>
#include <sys/time.h>
#include <sys/event.h>
#include <netinet/in.h>
#include <unistd.h>
#include <assert.h>

#include "web_server.h"
#include "util/resource_guard.h"
#include "log/log.h"

WebServer::WebServer(int port, bool optLinger, TriggerMode mode) : m_port(port), m_kq(mode), m_socket()
{
    // socket已经在参数列表中初始化好了 开始设置属性
    this->m_socket.setLinger(optLinger);
    this->m_socket.setReuseAddr(true);
    this->m_socket.bind(port);
    this->m_socket.listen();
}
WebServer::~WebServer()
{
    // RAII自动管理资源
}
void WebServer::run()
{
    bool stop_server = false;
    while (!stop_server)
    {
        // MY_LOG_INFO("服务运行中 socket是{0} kqueue是{1}", this->m_socketId->get(), this->m_kqId->get());
    }
}

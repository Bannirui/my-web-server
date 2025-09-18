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
#include "util/fd_util.h"

WebServer::WebServer(int port, bool optLinger, TriggerMode mode) : m_port(port), m_kq(mode), m_listenSocket()
{
    // socket已经在参数列表中初始化好了 开始设置属性
    this->m_listenSocket.setLinger(optLinger);
    this->m_listenSocket.setReuseAddr(true);
    this->m_listenSocket.setNonBlocking(true);
    this->m_listenSocket.bind(port);
    this->m_listenSocket.listen();
    // socket注册到selector
    this->m_kq.addReadEvent(this->m_listenSocket.getFd(), false);
    // 在当前系统退化成单向通道 读写都是非阻塞 但是只注册读端到selector
    int ret = socketpair(PF_UNIX, SOCK_STREAM, 0, m_pipefd);
    assert(ret != -1);
    // 写端 显式设置非阻塞
    FdUtil::setNonBlocking(m_pipefd[1]);
    // 读端会在注册selector的时候被设置成非阻塞
    this->m_kq.addReadEvent(m_pipefd[0]);
    // 在Linux/Unix下 如果往一个已经关闭的socket写数据 会触发SIGPIPE 默认行为是进程直接退出 所以必须忽略
    // 否则服务器写数据时会被kill掉 在事件循环中拿到这个信号就忽略实现忽略写关闭错误
    this->m_kq.addSignal(SIGPIPE);
    // 定时信号 捕获SIGALRM信号 配合selector实现定时任务的处理
    this->m_kq.addSignal(SIGALRM);
    // 捕获SIGTERM终止信号 这样服务器可以优雅关闭 而不是被系统直接kill
    this->m_kq.addSignal(SIGTERM);
    // 设置一个定时器(秒级) 到时间后会发SIGALRM信号 实现定时任务
    alarm(COMMON_FILE_INTERVAL_SECONDS);
}
WebServer::~WebServer()
{
    // RAII自动管理资源
    if (m_pipefd[0] != -1)
    {
        close(m_pipefd[0]);
    }
    if (m_pipefd[1] != -1)
    {
        close(m_pipefd[1]);
    }
}

void WebServer::run()
{
    bool          timeout     = false;
    bool          stop_server = false;
    struct kevent events[MAX_EVENT_NUMBER];
    while (!stop_server)
    {
        // kevent等待事件
        int number = kevent(m_kq.getFd(), nullptr, 0, events, MAX_EVENT_NUMBER, nullptr);
        if (number < 0 && errno != EINTR)
        {
            MY_LOG_ERROR("kqueue获取就绪事件失败");
            break;
        }
        for (int i = 0; i < number; i++)
        {
            int sockfd = static_cast<int>(events[i].ident);
            if (sockfd == this->m_listenSocket.getFd() && events[i].filter == EVFILT_READ)
            {
                // 处理新到的客户连接
                if (!this->processClient())
                {
                    continue;
                }
            }
            else if (events[i].flags & EV_EOF || events[i].flags & EV_ERROR)
            {
                // 服务器端关闭连接 移除对应的定时器
                // todo
            }
            else if ((sockfd == m_pipefd[0]) && events[i].filter == EVFILT_READ)
            {
                // 处理信号
                if (!processSig(timeout, stop_server))
                {
                    MY_LOG_ERROR("processSig失败");
                }
            }
            else if (events[i].filter == EVFILT_READ)
            {
                // 可读事件
                processRead(sockfd);
            }
            else if (events[i].filter == EVFILT_WRITE)
            {
                // 可写事件
                processWrite(sockfd);
            }
        }
        if (timeout)
        {
            MY_LOG_INFO("信号处理超时");
            // todo
            timeout = false;
        }
    }
}
bool WebServer::processClient()
{
    // todo
    return true;
}
void WebServer::processRead(uint32_t fd)
{
    // todo
}
void WebServer::processWrite(uint32_t fd)
{
    // todo
}
bool WebServer::processSig(bool& timeout, bool& stopServer)
{
    // todo
    return true;
}

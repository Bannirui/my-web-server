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
#include "log/log.h"
#include "util/fd_util.h"
#include "my_http.h"

WebServer::WebServer(int port, bool optLinger, TriggerMode mode) : m_port(port), m_kq(mode)
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
    struct sockaddr_in client_address;
    socklen_t          client_addrlength = sizeof(client_address);
    if (this->m_kq.LT_Mode())
    {
        // 水平触发
        int connfd = accept(this->m_listenSocket.getFd(), (struct sockaddr*)&client_address, &client_addrlength);
        if (connfd < 0)
        {
            MY_LOG_ERROR("获取客户端连接 拿到的socket是{}异常", connfd);
            return false;
        }
        if (my_http::s_user_count >= MAX_FD)
        {
            sendToSocket(connfd, "internal server busy", [](uint32_t socketFd) { close(socketFd); });
            MY_LOG_INFO(
                "当前客户端连接对应的socket是{} 已经建立的连接数{} 系统设置的连接数上限是{} 等等连接资源释放出来",
                connfd, my_http::s_user_count, MAX_FD);
            return false;
        }
        // 新连接加入kqueue
        struct kevent ev;
        EV_SET(&ev, connfd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
        if (kevent(this->m_kq.getFd(), &ev, 1, nullptr, 0, nullptr) == -1)
        {
            MY_LOG_ERROR("把客户端连接加到kq{}失败{}", this->m_kq.getFd(), errno);
            close(connfd);
            return false;
        }
        // todo 新建连接
    }
    else
    {
        // 边缘触发
        while (1)
        {
            int connfd = accept(this->m_listenSocket.getFd(), (struct sockaddr*)&client_address, &client_addrlength);
            if (connfd < 0)
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK) break;  // 已经没有可接收的连接
                MY_LOG_ERROR("获取客户端连接 拿到的socket是{}异常", connfd);
                break;
            }
            if (my_http::s_user_count >= MAX_FD)
            {
                sendToSocket(connfd, "internal server busy", [](uint32_t socketFd) { close(socketFd); });
                MY_LOG_INFO(
                    "当前客户端连接对应的socket是{} 已经建立的连接数{} 系统设置的连接数上限是{} 等等连接资源释放出来",
                    connfd, my_http::s_user_count, MAX_FD);
                break;
            }
            // 新连接加入kqueue(ET模式)
            struct kevent ev;
            EV_SET(&ev, connfd, EVFILT_READ, EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0, NULL);
            if (kevent(this->m_kq.getFd(), &ev, 1, nullptr, 0, nullptr) == -1)
            {
                MY_LOG_ERROR("把客户端连接加到kq{}失败{}", this->m_kq.getFd(), errno);
                close(connfd);
                continue;
            }
            // todo 新建连接
        }
        return false;
    }
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
void WebServer::sendToSocket(uint32_t fd, const char* msg, std::function<void(uint32_t)> postFn)
{
    send(fd, msg, strlen(msg), 0);
    if (postFn)
    {
        postFn(fd);
    }
}

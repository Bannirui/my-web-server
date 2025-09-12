//
// Created by rui ding on 2025/9/12.
//

#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <cassert>
#include <stdexcept>
#include <netinet/in.h>
#include <sys/fcntl.h>

#include "my_socket.h"
#include "log/log.h"
#include "util/fd_util.h"

MySocket::MySocket()
    : m_fd(socket(PF_INET, SOCK_STREAM, 0),
           [](int fd)
           {
               if (fd != -1)
               {
                   if (::close(fd) == -1)
                   {
                       MY_LOG_ERROR("关闭socket{}失败", fd);
                   }
                   else
                   {
                       MY_LOG_INFO("socket{}关闭", fd);
                   }
               }
           })
{
    if (m_fd.get() == -1)
    {
        throw std::runtime_error("Failed to create socket");
    }
    MY_LOG_INFO("socket{}创建成功", m_fd.get());
}

void MySocket::setLinger(bool enable, int timeoutSec) const
{
    struct linger opt;
    if (enable)
    {
        opt.l_onoff  = 1;
        opt.l_linger = timeoutSec;
        MY_LOG_INFO("设置socket优雅关闭{}s", timeoutSec);
    }
    else
    {
        opt.l_onoff  = 0;
        opt.l_linger = 1;
        MY_LOG_INFO("设置socket立即关闭");
    }
    setsockopt(m_fd.get(), SOL_SOCKET, SO_LINGER, &opt, sizeof(opt));
}

void MySocket::setNonBlocking(bool nonBlocking) const
{
    int flags = fcntl(this->m_fd.get(), F_GETFL, 0);
    if (flags == -1)
    {
        throw std::system_error(errno, std::generic_category(), "fcntl系统调用失败");
    }
    if (nonBlocking)
    {
        FdUtil::setNonBlocking(this->m_fd.get());
    }
    else
    {
        if (fcntl(this->m_fd.get(), F_SETFL, flags & ~O_NONBLOCK) == -1)
        {
            throw std::system_error(errno, std::generic_category(), "fcntl系统调用设置socket阻塞失败");
        }
    }
}

void MySocket::setReuseAddr(bool enable) const
{
    int flag = enable ? 1 : 0;
    setsockopt(m_fd.get(), SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    MY_LOG_INFO("设置socket端口复用{}", enable);
}

void MySocket::bind(int port)
{
    struct sockaddr_in address;
    std::memset(&address, 0, sizeof(address));
    // ipv4
    address.sin_family = AF_INET;
    // 绑定到所有可用网卡
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    // 绑定到指定的端口
    address.sin_port = htons(port);
    int ret          = ::bind(m_fd.get(), (struct sockaddr*)&address, sizeof(address));
    assert(ret >= 0);
    MY_LOG_INFO("socket{}绑定端口{}", m_fd.get(), port);
}

void MySocket::listen(int backlog) const
{
    int ret = ::listen(m_fd.get(), backlog);
    assert(ret >= 0);
    MY_LOG_INFO("socket{}开始监听 backlog={}", m_fd.get(), backlog);
}
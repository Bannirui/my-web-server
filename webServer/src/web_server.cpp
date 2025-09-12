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

WebServer::WebServer(int port, bool optLinger) : m_port(port), m_optLinger(optLinger)
{
    // socket
    this->m_socketId = new ResourceGuard<int>(socket(PF_INET, SOCK_STREAM, 0),
                                              [](int fd)
                                              {
                                                  if (fd != -1)
                                                  {
                                                      if (close(fd) == -1)
                                                      {
                                                          perror("close");
                                                      }
                                                      else
                                                      {
                                                          MY_LOG_INFO("socket{}关闭", fd);
                                                      }
                                                  }
                                              });
    assert(this->m_socketId->get() != -1);
    MY_LOG_INFO("socket{}创建成功", this->m_socketId->get());
    // 设置socket优雅关闭
    if (this->m_optLinger)
    {
        MY_LOG_INFO("设置socket优雅关闭");
        // 等1s给内核个缓冲时间
        struct linger tmp = {1, 1};
        setsockopt(this->m_socketId->get(), SOL_SOCKET, SO_LINGER, &tmp, sizeof(tmp));
    }
    else
    {
        MY_LOG_INFO("设置socket立即关闭");
        struct linger tmp = {0, 1};
        setsockopt(this->m_socketId->get(), SOL_SOCKET, SO_LINGER, &tmp, sizeof(tmp));
    }
    // 允许端口复用
    MY_LOG_INFO("设置socket端口复用");
    int flag = 1;
    setsockopt(this->m_socketId->get(), SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    // socket转主动监听 填充结构
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    // ipv4
    address.sin_family = AF_INET;
    // 绑定到所有可用网卡
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    // 绑定到指定的端口上
    address.sin_port = htons(this->m_port);
    int ret          = -1;
    // 绑定端口
    ret = bind(this->m_socketId->get(), (struct sockaddr*)&address, sizeof(address));
    assert(ret >= 0);
    MY_LOG_INFO("socket{}绑定端口{}", this->m_socketId->get(), this->m_port);
    // 开始监听
    int backlog = 5;
    ret         = listen(this->m_socketId->get(), backlog);
    assert(ret >= 0);
    MY_LOG_INFO("socket{}监听端口{}", this->m_socketId->get(), this->m_port);
    // kqueue
    this->m_kqId = new ResourceGuard<int>(kqueue(),
                                          [](int fd)
                                          {
                                              if (close(fd) == -1)
                                              {
                                                  perror("close");
                                              }
                                              else
                                              {
                                                  MY_LOG_INFO("socket{}关闭", fd);
                                              }
                                          });
    assert(this->m_kqId->get() != -1);
    MY_LOG_INFO("kqueue{}创建成功", this->m_kqId->get());
}
WebServer::~WebServer()
{
    delete m_socketId;
    delete m_kqId;
}
void WebServer::run()
{
    bool stop_server = false;
    while (!stop_server)
    {
        // MY_LOG_INFO("服务运行中 socket是{0} kqueue是{1}", this->m_socketId->get(), this->m_kqId->get());
    }
}

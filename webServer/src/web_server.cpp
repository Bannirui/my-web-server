//
// Created by rui ding on 2025/9/11.
//

#include <iostream>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/event.h>
#include <unistd.h>
#include <assert.h>

#include "web_server.h"
#include "resource_guard.h"

WebServer::WebServer() {}
WebServer::~WebServer()
{
    delete m_socketId;
    delete m_kqId;
}
void WebServer::init(int port) {}
void WebServer::listen()
{
    // socket
    this->m_socketId = new ResourceGuard<int>(socket(PF_INET, SOCK_STREAM, 0),
                                              [](int fd)
                                              {
                                                  if (fd != -1)
                                                  {
                                                      if (close(fd) == -1) perror("close");
                                                  }
                                              });
    assert(this->m_socketId->get() != -1);
    // kqueue
    this->m_kqId = new ResourceGuard<int>(kqueue(),
                                          [](int fd)
                                          {
                                              if (fd != -1)
                                              {
                                                  if (close(fd) == -1) perror("close");
                                              }
                                          });
    assert(this->m_kqId->get() != -1);
}
void WebServer::run()
{
    bool stop_server = false;
    while (!stop_server)
    {
        std::cout << "服务运行中"
                  << "socket是" << this->m_socketId->get() << " kqueue的id是" << this->m_kqId->get() << std::endl;
    }
}

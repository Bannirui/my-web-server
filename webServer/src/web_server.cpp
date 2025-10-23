//
// Created by rui ding on 2025/9/11.
//
#include <sys/epoll.h>

#include "web_server.h"
#include "log.h"
#include "event_loop.h"
#include "http_connect.h"

#include <fcntl.h>

my_ws::WebServer::WebServer() {}

my_ws::WebServer::~WebServer() {}

void my_ws::WebServer::run()
{
    try
    {
        Socket     listenSock = Socket::ListenTcp(9527);
        EventLoop  loop;
        ThreadPool pool(4);

        std::unordered_map<int, std::unique_ptr<HttpConnect>> conns;

        // accept handler
        loop.AddFd(listenSock.Fd(), EPOLLIN,
                   [&](uint32_t ev)
                   {
                       while (true)
                       {
                           sockaddr_in cli{};
                           int         serverSock = Socket::AcceptTcp(listenSock.Fd(), cli);
                           if (serverSock < 0)
                           {
                               if (errno == EAGAIN || errno == EWOULDBLOCK) break;
                               perror("accept");
                               break;
                           }
                           // set non-blocking
                           int flags = fcntl(serverSock, F_GETFL, 0);
                           fcntl(serverSock, F_SETFL, flags | O_NONBLOCK);
                           // create connection object
                           conns[serverSock] = std::make_unique<HttpConnect>(serverSock, loop);
                           // register fd
                           loop.AddFd(serverSock, EPOLLIN | EPOLLET,
                                      [&, serverSock](uint32_t ev)
                                      {
                                          if (ev & EPOLLIN)
                                          {
                                              if (conns[serverSock])
                                              {
                                                  conns[serverSock]->OnRead();
                                              }
                                          }
                                          if (ev & EPOLLOUT)
                                          {
                                              if (conns[serverSock])
                                              {
                                                  conns[serverSock]->OnWrite();
                                              }
                                          }
                                          if (ev & (EPOLLHUP | EPOLLERR))
                                          {
                                              conns.erase(serverSock);
                                          }
                                      });
                       }
                   });

        LOG_INFO("start event loop");
        loop.Run();
    }
    catch (std::exception &ex)
    {
        LOG_ERROR("err: {}", ex.what());
    }
}

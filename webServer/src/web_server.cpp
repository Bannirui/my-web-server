//
// Created by rui ding on 2025/9/11.
//
#include "web_server.h"

#include <sys/epoll.h>
#include <fcntl.h>

#include "log.h"
#include "event_loop.h"

#include <unistd.h>

my_ws::WebServer::WebServer(uint16_t port)
    : _listenSock(Socket::ListenTcp(port)),
      _eventLoop(std::make_unique<EventLoop>()),
      _threadPool(THREAD_POOL_THREADS),
      _connPool(std::make_unique<ConnectionPool>(*_eventLoop))
{
    _listenSock.SetNonBlocking(true);
}

my_ws::WebServer::~WebServer() = default;

void my_ws::WebServer::Run()
{
    try
    {
        // register accept handler
        _eventLoop->AddFd(_listenSock.Fd(), EPOLLIN, [this](uint32_t evs) { onAccept(); });
        // start connection pool timer(for cleanup)
        _connPool->StartTimer();
        LOG_INFO("event loop start");
        _eventLoop->Run();
    }
    catch (std::exception &ex)
    {
        LOG_ERROR("err: {}", ex.what());
    }
}

void my_ws::WebServer::onAccept()
{
    while (true)
    {
        sockaddr_in cli{};
        int         serverSock = Socket::AcceptTcp(_listenSock.Fd(), cli);
        if (serverSock < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK) break;
            LOG_ERROR("accept error: {}", strerror(errno));
            break;
        }
        // set non-blocking
        int flags = fcntl(serverSock, F_GETFL, 0);
        if (flags == -1)
        {
            LOG_ERROR("fcntl failed: {}", strerror(errno));
            close(serverSock);
            continue;
        }
        if (fcntl(serverSock, F_SETFL, flags | O_NONBLOCK) == -1)
        {
            LOG_ERROR("fcntl failed: {}", strerror(errno));
            close(serverSock);
            continue;
        }
        // add to connection pool
        _connPool->Add(serverSock);
    }
}

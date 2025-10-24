//
// Created by dingrui on 2025/10/22.
//
#include <ostream>
#include <unistd.h>
#include <sys/syslog.h>

#include "http_connect.h"

#include "event_loop.h"
#include "log.h"

#include <sys/epoll.h>

my_ws::HttpConnect::HttpConnect(int fd, EventLoop &loop) : _fd(fd), _eventLoop(loop) {}

my_ws::HttpConnect::~HttpConnect()
{
    if (_fd >= 0)
    {
        close(_fd);
    }
}

void my_ws::HttpConnect::OnRead()
{
    char buf[4096];
    while (true)
    {
        ssize_t n = read(_fd, buf, sizeof(buf));
        if (n > 0)
        {
            _in.append(buf, buf + n);
        }
        else if (n == 0)
        {
            // closed
            LOG_ERROR("read closed");
            close(_fd);
            _fd = -1;
            return;
        }
        else
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                break;
            }
            perror("read");
            close(_fd);
            _fd = -1;
            return;
        }
    }
    // respond if have sth
    if (!_in.empty())
    {
        LOG_INFO("{} get {}", Fd(), _in);
        _out = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nHello World";
    }
}

void my_ws::HttpConnect::OnWrite()
{
    while (!_out.empty())
    {
        LOG_INFO("ready send msg to client, {}", _out);
        ssize_t n = write(_fd, _out.data(), _out.size());
        if (n > 0)
        {
            _out.erase(0, n);
        }
        else
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                break;
            }
            perror("write");
            close(_fd);
            _fd = -1;
            return;
        }
    }
    // finish sending, disable EPOLLOUT
    _eventLoop.ModifyFd(_fd, EPOLLIN | EPOLLET);
}

int my_ws::HttpConnect::Fd() const
{
    return _fd;
}

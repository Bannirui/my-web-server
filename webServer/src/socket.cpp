#include "socket.h"

#include <cstring>
#include <fcntl.h>
#include <stdexcept>
#include <unistd.h>

my_ws::Socket::Socket(int fd) noexcept : _fd(fd) {}

my_ws::Socket::~Socket()
{
    if (_fd >= 0)
    {
        close(_fd);
    }
}

my_ws::Socket::Socket(Socket &&sock) noexcept : _fd(sock._fd)
{
    sock._fd = -1;
}

my_ws::Socket &my_ws::Socket::operator=(Socket &&sock) noexcept
{
    if (this != &sock)
    {
        if (_fd >= 0)
        {
            close(_fd);
        }
        _fd      = sock._fd;
        sock._fd = -1;
    }
    return *this;
}

my_ws::Socket my_ws::Socket::ListenTcp(int port, int backlog)
{
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0)
    {
        throw std::runtime_error(std::string("socket: ") + strerror(errno));
    }
    int opt = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(port);
    if (bind(s, (sockaddr *)&addr, sizeof(addr)) < 0)
    {
        close(s);
        throw std::runtime_error(std::string("bind: ") + strerror(errno));
    }
    if (listen(s, backlog) < 0)
    {
        close(s);
        throw std::runtime_error(std::string("listen: ") + strerror(errno));
    }
    Socket sock(s);
    sock.SetNonBlocking(true);
    return sock;
}

int my_ws::Socket::AcceptTcp(int listenFd, sockaddr_in &outAddr)
{
    socklen_t addrLen = sizeof(outAddr);
    int       c       = accept(listenFd, (struct sockaddr *)&outAddr, &addrLen);
    return c;
}

int my_ws::Socket::Fd() const noexcept
{
    return _fd;
}

void my_ws::Socket::SetNonBlocking(bool on)
{
    if (_fd < 0)
    {
        return;
    }
    int flags = fcntl(_fd, F_GETFL, 0);
    if (flags == -1)
    {
        return;
    }
    if (on)
    {
        flags |= O_NONBLOCK;
    }
    else
    {
        flags &= ~O_NONBLOCK;
    }
    fcntl(_fd, F_SETFL, flags);
}

#pragma once

#include <netinet/in.h>

namespace my_ws
{
    class Socket
    {
    public:
        explicit Socket(int fd) noexcept;
        ~Socket();
        Socket(const Socket &)            = delete;
        Socket &operator=(const Socket &) = delete;
        Socket(Socket &&) noexcept;
        Socket &operator=(Socket &&) noexcept;

        static Socket ListenTcp(int port, int backlog = 128);
        /**
         * @return system allocates new socket fd, pair (server socket, client socket), it aims to communicate with
         * client
         */
        static int    AcceptTcp(int listenFd, sockaddr_in &outAddr);
        int           Fd() const noexcept;
        void          SetNonBlocking(bool on = true);

    private:
        int _fd{-1};
    };
}  // namespace my_ws
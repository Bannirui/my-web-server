//
// Created by dingrui on 2025/10/22.
//
#include <string>

namespace my_ws
{
    struct EventLoop;

    class HttpConnect
    {
    public:
        HttpConnect(int fd, EventLoop &loop);
        ~HttpConnect();

        void OnRead();
        void OnWrite();
        int  Fd() const;

    private:
        int         _fd{-1};
        EventLoop  &_eventLoop;
        std::string _in, _out;
    };
}  // namespace my_ws
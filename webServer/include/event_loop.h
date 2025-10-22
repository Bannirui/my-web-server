#pragma once

#include <functional>
#include <vector>

namespace my_ws
{
    /**
     * based on the system selector
     * <ul>
     *   <li>epoll on linux</li>
     *   <li>kqueue on mac</li>
     * </ul>
     */
    class EventLoop
    {
    public:
        EventLoop();
        ~EventLoop();

        using FdCallback = std::function<void(u_int32_t)>;

        void Run();
        void Stop();

        /**
         * events register/modify/remove
         * @param fd to watch which socket
         * @param events what actually watching for
         * @param cb callback if fd ready for events
         */
        void AddFd(int fd, u_int32_t events, FdCallback cb);
        void ModifyFd(int fd, u_int32_t events);
        void DelFd(int fd);

    private:
        int  _selectorId{-1};
        bool _running{false};
        struct Entry
        {
            int        fd;
            u_int32_t  events;
            FdCallback cb;
        };
        std::vector<Entry> _handlers;
    };
}  // namespace my_ws
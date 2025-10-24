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
        /**
         * events the events type wanna be watched
         */
        using FdCallback = std::function<void(uint32_t events)>;

        struct Entry
        {
            int        fd;
            u_int32_t  events;
            FdCallback cb;
        };

        EventLoop();
        ~EventLoop();

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
        int                            _selectorId{-1};
        bool                           _running{false};
        // key is fd, and val is the Entry
        std::unordered_map<int, Entry> _handlers;
    };
}  // namespace my_ws
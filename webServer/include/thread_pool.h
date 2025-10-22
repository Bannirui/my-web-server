#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

namespace my_ws
{
    class ThreadPool
    {
    public:
        ThreadPool(size_t n = std::thread::hardware_concurrency());
        ~ThreadPool();

        void Submit(std::function<void()> job);

    private:
        std::vector<std::thread>          _workers;
        std::queue<std::function<void()>> _queue;
        std::mutex                        _mutex;
        std::condition_variable           _condition;
        bool                              _stop{false};
    };
}  // namespace my_ws
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
        /**
         * @param n how many threads
         */
        ThreadPool(size_t n = std::thread::hardware_concurrency());
        ~ThreadPool();

        /**
         * @param task submit a task to thread pool
         */
        void Submit(std::function<void()> task);

    private:
        std::vector<std::thread>          _threads;
        std::queue<std::function<void()>> _tasks;
        std::mutex                        _mutex;
        std::condition_variable           _condition;
        bool                              _stop{false};
    };
}  // namespace my_ws
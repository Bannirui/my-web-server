#include "thread_pool.h"

my_ws::ThreadPool::ThreadPool(size_t n) : _stop(false)
{
    for (size_t i = 0; i < n; ++i)
    {
        _workers.emplace_back(
            [this]()
            {
                while (true)
                {
                    std::function<void()> job;
                    {
                        std::unique_lock<std::mutex> lk(_mutex);
                        _condition.wait(lk, [this]() { return _stop || !_queue.empty(); });
                        if (_stop && _queue.empty())
                        {
                            return;
                        }
                        job = std::move(_queue.front());
                        _queue.pop();
                    }
                    try
                    {
                        job();
                    }
                    catch (...)
                    {
                    }
                }
            });
    }
}

my_ws::ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lk(_mutex);
        _stop = true;
    }
    _condition.notify_all();
    for (auto &t : _workers)
    {
        if (t.joinable())
        {
            t.join();
        }
    }
}

void my_ws::ThreadPool::Submit(std::function<void()> job)
{
    {
        std::unique_lock<std::mutex> lk(_mutex);
        _queue.push(std::move(job));
    }
    _condition.notify_one();
}

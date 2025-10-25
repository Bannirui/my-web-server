#include "thread_pool.h"

my_ws::ThreadPool::ThreadPool(size_t n) : _stop(false)
{
    for (size_t i = 0; i < n; ++i)
    {
        _threads.emplace_back(
            [this]()
            {
                while (true)
                {
                    std::function<void()> job;
                    {
                        std::unique_lock<std::mutex> lk(_mutex);
                        _condition.wait(lk, [this]() { return _stop || !_tasks.empty(); });
                        if (_stop && _tasks.empty())
                        {
                            return;
                        }
                        job = std::move(_tasks.front());
                        _tasks.pop();
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
    for (auto &t : _threads)
    {
        if (t.joinable())
        {
            t.join();
        }
    }
}

void my_ws::ThreadPool::Submit(std::function<void()> task)
{
    {
        std::unique_lock<std::mutex> lk(_mutex);
        _tasks.push(std::move(task));
    }
    _condition.notify_one();
}

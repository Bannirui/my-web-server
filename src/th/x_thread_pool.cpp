//
// Created by rui ding on 2026/1/21.
//

#include "th/x_thread_pool.h"

#include "x_log.h"

XThreadPool::XThreadPool(size_t n)
{
    for (size_t i = 0; i < n; ++i)
    {
        this->m_workers.emplace_back(&XThreadPool::worker, this);
    }
}

XThreadPool::~XThreadPool()
{
    {
        std::lock_guard<std::mutex> lk(this->m_mtx);
        this->m_stop = true;
    }
    this->m_cv.notify_all();
    for (auto &t : this->m_workers)
    {
        t.join();
    }
}

void XThreadPool::Submit(std::function<void()> task)
{
    {
        std::lock_guard<std::mutex> lk(this->m_mtx);
        this->m_tasks.push(std::move(task));
    }
    this->m_cv.notify_one();
}

void XThreadPool::worker()
{
    for (;;)
    {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lk(this->m_mtx);
            this->m_cv.wait(lk, [&] { return this->m_stop || !this->m_tasks.empty(); });
            if (this->m_stop && this->m_tasks.empty())
            {
                return;
            }
            task = std::move(this->m_tasks.front());
            this->m_tasks.pop();
        }
        XLOG_INFO("从线程池拿到任务 开始执行");
        task();
    }
}

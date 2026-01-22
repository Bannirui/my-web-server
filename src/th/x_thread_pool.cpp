//
// Created by rui ding on 2026/1/21.
//

#include "th/x_thread_pool.h"

#include "log/x_log.h"

XThreadPool::XThreadPool(size_t n)
{
    for (size_t i = 0; i < n; ++i)
    {
        this->m_workers.emplace_back(&XThreadPool::worker, this);
    }
}

XThreadPool::~XThreadPool()
{
    this->Shutdown();
}

bool XThreadPool::Submit(std::function<void()> task)
{
    {
        std::lock_guard<std::mutex> lk(this->m_mtx);
        if (this->m_stop)
        {
            XLOG_WARN("ThreadPool stopped, reject task");
            return false;
        }
        this->m_tasks.push(std::move(task));
    }
    this->m_cv.notify_one();
    return true;
}

void XThreadPool::Shutdown()
{
    {
        std::lock_guard<std::mutex> lk(this->m_mtx);
        if (m_stop)
        {
            return;
        }
        m_stop = true;
    }
    m_cv.notify_all();
    for (auto &t : m_workers)
    {
        if (t.joinable())
        {
            t.join();
        }
    }
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
        try
        {
            XLOG_INFO("线程池开始执行任务");
            task();
        }
        catch (const std::exception &e)
        {
            XLOG_ERROR("线程池任务异常: {}", e.what());
        }
        catch (...)
        {
            XLOG_ERROR("线程池任务发生未知异常");
        }
    }
}

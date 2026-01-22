//
// Created by rui ding on 2026/1/21.
//

#pragma once

#include <functional>
#include <queue>
#include <thread>
#include <condition_variable>
#include <mutex>

class XThreadPool
{
public:
    explicit XThreadPool(size_t n);
    ~XThreadPool();

    bool Submit(std::function<void()> task);
    // 等线程池里面已经存在的任务执行完
    void Shutdown();

private:
    void worker();

    bool                              m_stop{false};
    std::queue<std::function<void()>> m_tasks;
    std::mutex                        m_mtx;
    std::condition_variable           m_cv;
    std::vector<std::thread>          m_workers;
};

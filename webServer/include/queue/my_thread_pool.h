//
// Created by rui ding on 2025/9/24.
//

#pragma once

#include <cstdint>
#include <list>
#include <sys/_pthread/_pthread_t.h>

#include "concurrent/my_lock.h"

/**
 * 使用一个工作队列完全解除了主线程和工作线程的耦合关系
 * 主线程往工作队列中插入任务
 * 工作线程通过竞争来取得任务并执行它
 */
template <typename T>
class MyThreadPool
{
public:
    MyThreadPool(uint32_t thread_number = 8, uint32_t max_request = 65536);
    ~MyThreadPool();
    bool append(T* request, int state);
    bool append(T* request);

private:
    /**
     * 工作线程运行的函数
     * 从工作队列中取出任务并执行
     * @param arg
     * @return
     */
    static void* worker(void* arg);
    void         run();

private:
    uint32_t      m_thread_number;  // 线程池中的活跃线程数
    uint32_t      m_max_requests;   // 请求队列中允许的最大请求数
    pthread_t*    m_threads;        // 描述线程池的数组 其大小为m_thread_number
    std::list<T*> m_workQueue;      // 任务队列
    MyLock        m_queueLocker;    // 保护请求队列的互斥锁
};

template <typename T>
MyThreadPool<T>::MyThreadPool(uint32_t thread_number, uint32_t max_requests)
    : m_thread_number(thread_number), m_max_requests(max_requests), m_threads(NULL)
{
    // 创建线程
    m_threads = new pthread_t[m_thread_number];
    if (!m_threads) throw std::exception();
    for (int i = 0; i < thread_number; ++i)
    {
        if (pthread_create(m_threads + i, NULL, worker, this) != 0)
        {
            delete[] m_threads;
            throw std::exception();
        }
        if (pthread_detach(m_threads[i]))
        {
            delete[] m_threads;
            throw std::exception();
        }
    }
}

template <typename T>
MyThreadPool<T>::~MyThreadPool()
{
    delete[] m_threads;
}

template <typename T>
bool MyThreadPool<T>::append(T* request, int state)
{
    m_queuelocker.lock();
    if (m_workqueue.size() >= m_max_requests)
    {
        m_queuelocker.unlock();
        return false;
    }
    request->m_state = state;
    m_workqueue.push_back(request);
    m_queuelocker.unlock();
    m_queuestat.post();
    return true;
}

template <typename T>
bool MyThreadPool<T>::append(T* request)
{
    m_queueLocker.lock();
    if (m_workQueue.size() >= m_max_requests)
    {
        m_queueLocker.unlock();
        return false;
    }
    // 入队
    m_workQueue.push_back(request);
    m_queueLocker.unlock();
    m_queuestat.post();
    return true;
}

template <typename T>
void* MyThreadPool<T>::worker(void* arg)
{
    threadpool* pool = (threadpool*)arg;
    pool->run();
    return pool;
}

template <typename T>
void MyThreadPool<T>::run()
{
    while (true)
    {
        m_queuestat.wait();
        m_queuelocker.lock();
        if (m_workqueue.empty())
        {
            m_queuelocker.unlock();
            continue;
        }
        T* request = m_workqueue.front();
        m_workqueue.pop_front();
        m_queuelocker.unlock();
        if (!request) continue;
        if (1 == m_actor_model)
        {
            if (0 == request->m_state)
            {
                if (request->read_once())
                {
                    request->improv = 1;
                    connectionRAII mysqlcon(&request->mysql, m_connPool);
                    request->process();
                }
                else
                {
                    request->improv     = 1;
                    request->timer_flag = 1;
                }
            }
            else
            {
                if (request->write())
                {
                    request->improv = 1;
                }
                else
                {
                    request->improv     = 1;
                    request->timer_flag = 1;
                }
            }
        }
        else
        {
            connectionRAII mysqlcon(&request->mysql, m_connPool);
            request->process();
        }
    }
}

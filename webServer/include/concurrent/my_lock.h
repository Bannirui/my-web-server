//
// Created by rui ding on 2025/9/24.
//

#pragma once

#include <exception>
#include <pthread.h>
#include <sys/_pthread/_pthread_mutex_t.h>

/// 多线程迸发编程
/// 多线程同步 确保任一时刻只能有一个线程能进入关键代码段

/**
 * 信号量
 */
class MySem {
public:
    MySem() {
        m_sem = dispatch_semaphore_create(0);
        if (!m_sem) {
            throw std::runtime_error("dispatch_semaphore_create failed");
        }
    }

    MySem(int num) {
        m_sem = dispatch_semaphore_create(num);
        if (!m_sem) {
            throw std::runtime_error("dispatch_semaphore_create failed");
        }
    }

    ~MySem() {
        // dispatch_semaphore_t 在 ARC 下会自动释放，
        // 如果是纯 C++，用 CoreFoundation 管理：
        // dispatch_release(m_sem);  // macOS < 10.12 需要
    }

    bool wait() {
        return dispatch_semaphore_wait(m_sem, DISPATCH_TIME_FOREVER) == 0;
    }

    bool post() {
        dispatch_semaphore_signal(m_sem);
        return true;
    }

private:
    dispatch_semaphore_t m_sem;
};

/**
 * 互斥锁
 */
class MyLock
{
public:
    MyLock()
    {
        if (pthread_mutex_init(&m_mutex, NULL) != 0)
        {
            throw std::exception();
        }
    }

    ~MyLock() { pthread_mutex_destroy(&m_mutex); }

    bool             lock() { return pthread_mutex_lock(&m_mutex) == 0; }
    bool             unlock() { return pthread_mutex_unlock(&m_mutex) == 0; }
    pthread_mutex_t* get() { return &m_mutex; }

private:
    pthread_mutex_t m_mutex;
};

/**
 * 条件变量
 */
class MyCond
{
public:
    MyCond()
    {
        if (pthread_cond_init(&m_cond, NULL) != 0)
        {
            // pthread_mutex_destroy(&m_mutex);
            throw std::exception();
        }
    }
    ~MyCond()() { pthread_cond_destroy(&m_cond); }

    bool wait(pthread_mutex_t* m_mutex)
    {
        int ret = 0;
        // pthread_mutex_lock(&m_mutex);
        ret = pthread_cond_wait(&m_cond, m_mutex);
        // pthread_mutex_unlock(&m_mutex);
        return ret == 0;
    }
    /**
     * 带超时等待的版本
     */
    bool wait(pthread_mutex_t* m_mutex, struct timespec t)
    {
        int ret = 0;
        // pthread_mutex_lock(&m_mutex);
        ret = pthread_cond_timedwait(&m_cond, m_mutex, &t);
        // pthread_mutex_unlock(&m_mutex);
        return ret == 0;
    }
    bool signal() { return pthread_cond_signal(&m_cond) == 0; }
    bool broadcast() { return pthread_cond_broadcast(&m_cond) == 0; }

private:
    // static pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;
};

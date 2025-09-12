//
// Created by rui ding on 2025/9/12.
//

#pragma once
#include "util/resource_guard.h"

// selector的触发方式
enum class TriggerMode
{
    LEVEL,  // 水平触发
    EDGE,   // 边缘触发
};

// 对系统kqueue的封装
class MyKqueue
{
public:
    /**
     * @param mode selector的触发方式 跟os保持一致默认是水平触发
     */
    MyKqueue(TriggerMode mode = TriggerMode::LEVEL);
    // 禁止拷贝
    MyKqueue(const MyKqueue&)            = delete;
    MyKqueue& operator=(const MyKqueue&) = delete;
    // 允许移动
    MyKqueue(MyKqueue&&) noexcept            = default;
    MyKqueue& operator=(MyKqueue&&) noexcept = default;

    int getFd() const { return m_kq.get(); }

    void                       addReadEvent(int fd) const;
    void                       addWriteEvent(int fd) const;
    void                       removeEvent(int fd, int filter) const;
    std::vector<struct kevent> wait(int timoutMs = -1) const;

private:
    int getFlags() const;

private:
    // 用ResourceGuard自动管理fd
    ResourceGuard<int> m_kq;
    // selector的触发方式
    TriggerMode m_mode;
};

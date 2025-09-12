//
// Created by rui ding on 2025/9/12.
//

#pragma once
#include "util/resource_guard.h"

// 非IO事件借助selector实现定时任务
#define COMMON_FILE_INTERVAL_SECONDS 5

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

    /**
     * 向复用器注册读事件
     * @param fd
     * @param oneShot
     */
    void                       addReadEvent(int fd, bool oneShot = false) const;
    void                       addWriteEvent(int fd) const;
    void                       removeEvent(int fd, int filter) const;
    std::vector<struct kevent> wait(int timoutMs = -1) const;

    /**
     * 通过监听信号实现特定信号的处理 替代了信号回调函数的方式
     * @param sig 注册信号
     */
    void addSignal(int sig) const;
    // todo 事件循环中处理3个信号
    // SIGPIPE 忽略掉
    // SIGALARM 处理 实现定时任务
    // SIGTERM 处理 实现优雅退出
private:
    int getFlags() const;

private:
    // 用ResourceGuard自动管理fd
    ResourceGuard<int> m_kq;
    // selector的触发方式
    TriggerMode m_mode;
};

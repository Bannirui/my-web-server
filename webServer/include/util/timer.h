//
// Created by rui ding on 2025/9/23.
//

#pragma once
#include <functional>
#include <netinet/in.h>

class MyHttpConn;

/**
 * 定时器处理闲置连接
 * 由于非活跃连接占用了连接资源 严重影响服务器的性能
 * 实现一个服务器定时器 处理这种非活跃连接 释放连接资源
 * 利用alarm函数周期性地触发SIGALRM信号 该信号的信号处理函数利用管道通知主循环执行定时器链表上的定时任务
 * 1 统一事件源
 * 2 基于升序链表的定时器
 * 3 处理非活动连接
 * 在数组结构中扮演链表节点
 */
class Timer
{
public:
    Timer();
    Timer(time_t expire, MyHttpConn* conn);
    ~Timer();

    /**
     * 定时器
     */
    void Delete();

private:
    Timer* prev;
    Timer* next;

    // 定时器的到期时间
    time_t _expire;
    // 这个定时器负责管理哪个连接
    MyHttpConn* _conn;
};

/**
 * 管理定时器的链表结构
 */
class TimerLst
{
public:
    TimerLst();
    ~TimerLst();

    /**
     * 从链表中移除节点
     */
    void Del(Timer* timer);
private:
    Timer* head;
    Timer* tail;
};
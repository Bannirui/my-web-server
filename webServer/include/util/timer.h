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
 * 链表头尾是数据节点 不需要哨兵节点
 */
class Timer
{
public:
    Timer();
    Timer(time_t expire, MyHttpConn* conn);
    ~Timer();

    time_t GetExpire();
    void   SetExpire(time_t expire);

    /**
     * 定时器
     */
    void Delete();

public:
    Timer* _Prev;
    Timer* _Next;

private:
    // 定时器的到期时间
    time_t _expire;
    // 这个定时器负责管理哪个连接
    MyHttpConn* _conn;
};

/**
 * 管理定时器的链表结构
 * 按照定时器器的到期时间升序
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
    /**
     * 定时器发生了变化 刷新在链表中的位置
     * 比如 定时器的作用是回收闲置连接的 发现连接还在进行数据传输 就后移检测时间 需要重新刷新定时器在链表中的位置
     * @param timer 定时器
     */
    void Reset(Timer* timer);

private:
    /**
     * 向链表中添加定时器结点保证整个链表中定时器到期时间升序
     * @param timer 要添加的数据结点
     */
    void addTimer(Timer* timer);
    /**
     * 已经保证了(...head)是有序的情况下往链表添加节点
     * @param node 自[node...]开始插入timer保证[node...timer...]的有序
     * @param timer 要添加的节点
     */
    void addTimer(Timer* node, Timer* timer);

public:
    Timer* _Head;
    Timer* _Tail;
};
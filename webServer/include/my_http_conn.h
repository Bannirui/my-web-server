//
// Created by rui ding on 2025/9/23.
//

#pragma once

#include <netinet/in.h>

#include "my_kq.h"

class MyKqueue;
/**
 * 根据状态转移 通过主从状态机封装了http连接类
 * 主状态机在内部调用从状态机 从状态机将处理状态和数据传给主状态机
 * 1 客户端发出http连接请求
 * 2 从状态机读取数据 更新自身状态和接收数据 传给主状态机
 * 3 主状态机根据从状态机状态 更新自身状态 决定响应请求还是继续读取
 */
class MyHttpConn
{
public:
    MyHttpConn();
    ~MyHttpConn();

    /**
     *
     * @param sockfd accept在服务端复制出来的socket 负责跟客户端socket通信
     * @param addr 客户端socket信息
     */
    void Init(int sockfd, const sockaddr_in &addr, MyKqueue *selector);
    /**
     * 服务端的连接关闭
     * 1 从复用器上移除监听
     * 2 关闭服务端的socket
     * 3 更新活跃连接计数
     */
    void Close();
    int GetState() { return _state; }
    void SetState(int state) { _state = state; }
    int  GetImprov() const { return _improv; }
    void SetImprov(int flag) { _improv = flag; }
    int  GetTimerFlag() const { return _timerFlag; }
    void SetTimerFlag(int timerFlag) { _timerFlag = timerFlag; }

private:
    void init();

public:
    /**
     * 封装的客户端连接计数 每来一个tcp连接就封装一个 上限是系统socket fd的限制     /**
     * 连接数达到阈值就关闭当前进来的请求连接等待连接资源被释放出来
     */
    static int s_user_count;

private:
    // accept在服务端复制出来的socket 负责跟客户端进行通信
    uint32_t _serverSockFd;
    // 客户端socket的信息
    sockaddr_in _clientAddress;
    // 所有的连接都会注册到多路复用器上 关闭连接的时候也要从复用器上移除
    MyKqueue *_selector;
    // 0=读 1=写
    int _state;
    // 表示线程池已经受理了读写任务
    int _improv;
    // 已经给链接挂载了定时器受理读写请求
    int _timerFlag;
};

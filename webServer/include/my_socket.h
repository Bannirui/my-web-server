//
// Created by rui ding on 2025/9/12.
//

#pragma once

#include "util/resource_guard.h"

class MySocket
{
public:
    MySocket();
    ~MySocket() = default;

    // 禁止拷贝 允许移动
    MySocket(const MySocket&)                = delete;
    MySocket& operator=(const MySocket&)     = delete;
    MySocket(MySocket&&) noexcept            = default;
    MySocket& operator=(MySocket&&) noexcept = default;

    /**
     * socket短时间的优雅关闭
     * 立即关闭 close方法会立即返回 内核会尽力将缓冲区的数据发送出去但不保证成功
     * 优雅关闭 close方法会阻塞一定时间 在这个等待时间窗口期 内核如果把缓冲区数据发完了就会正常关闭
     * 如果在等待窗口期缓冲区数据还没发完就会被直接丢弃
     * @param enable true-开启优雅关闭socket false-立即关闭socket
     * @param timeoutSec 阻塞等待的窗口期
     */
    void setLinger(bool enable, int timeoutSec = 1) const;
    /**
     * 端口重用
     */
    void setReuseAddr(bool enable) const;
    /**
     * 被动socket转换主动socket 绑定端口
     */
    void bind(int port);
    /**
     * 开启监听端口
     * @param backlog 对应os的backlog
     */
    void listen(int backlog = 5) const;

    int getFd() const { return m_fd.get(); }

private:
    ResourceGuard<int> m_fd;
};
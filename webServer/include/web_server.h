//
// Created by rui ding on 2025/9/11.
//

#pragma once

#include "my_kq.h"

class WebServer
{
public:
    /**
     * @param port 服务端口
     * @param optLinger 开关开启socket的优雅关闭
     * @param mode 是否使用边缘触发(默认水平触发)
     */
    WebServer(int port, bool optLinger, TriggerMode mode = TriggerMode::LEVEL);
    ~WebServer();
    void run();

private:
    int m_port;
    // socket fd
    ResourceGuard<int>* m_socketId;
    /**
     * 开关实现socket短时间的优雅关闭
     * true-开启优雅关闭socket
     * false-立即关闭socket
     * 立即关闭 close方法会立即返回 内核会尽力将缓冲区的数据发送出去但不保证成功
     * 优雅关闭 close方法会阻塞一定时间 在这个等待时间窗口期 内核如果把缓冲区数据发完了就会正常关闭
     * 如果在等待窗口期缓冲区数据还没发完就会被直接丢弃
     */
    bool m_optLinger{false};
    MyKqueue m_kq;
};

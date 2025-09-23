//
// Created by rui ding on 2025/9/11.
//

#pragma once

#include "my_http_conn.h"
#include "my_kq.h"
#include "my_socket.h"
#include "util/timer.h"

#define MAX_EVENT_NUMBER (1 << 10)
// 最大的文件描述符
#define MAX_FD 65536
// 定时器工作的时间刻度
#define TIMER_INTERVAL 5

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
    /**
     * 有新的客户端连接请求进来
     */
    bool processClient();
    void processRead(uint32_t fd);
    void processWrite(uint32_t fd);
    bool processSig(bool& timeout, bool& stopServer);

private:
    /**
     * 向socket发送消息
     * @param fd 向哪个socket发消息
     * @param msg 要发送的消息
     * @param postFn 回调函数
     */
    void sendToSocket(uint32_t fd, const char* msg, std::function<void(uint32_t)> postFn);

private:
    int m_port;
    // 多路复用器
    MyKqueue _selector;
    MySocket m_listenSocket;
    // 共享信号 类似golang中的channel机制 在本地创建一条两端连接的套接字通信 [0]负责读 [1]负责写
    // 这个生产者消费者模型 系统层面是全双工模式
    int m_pipefd[2]{-1, -1};
    // 连接池 缓存着对客户端连接的封装 索引号是当前服务端accept开辟出来的socket fd
    MyHttpConn* _userConns;
    // 管理连接池的连接 定时器释放闲置连接资源
    Timer** _userConnTimers;
};

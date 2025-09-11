//
// Created by rui ding on 2025/9/11.
//

#pragma once

#define TRY_CLOSE(fd)            \
    {                            \
        if (fd != -1) close(fd); \
    }

template <typename T>
class ResourceGuard;  // 前向声明

class WebServer
{
public:
    WebServer();
    ~WebServer();
    /**
     * @param port 服务端口
     */
    void init(int port);
    void listen();
    void run();

private:
    int m_port;
    // socket fd
    ResourceGuard<int>* m_socketId;
    // mac io selector, kqueue
    ResourceGuard<int>* m_kqId;
};

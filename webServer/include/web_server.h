//
// Created by rui ding on 2025/9/11.
//

#pragma once

#include "my_kq.h"
#include "my_socket.h"

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
    MyKqueue m_kq;
    MySocket m_socket;
};

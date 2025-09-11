//
// Created by rui ding on 2025/9/11.
//

#pragma once

class WebServer
{
public:
    WebServer();
    ~WebServer();
    /**
     * @param port 服务端口
     */
    void init(int port);
    void run();

private:
    void listen();

public:
    int m_port;
};

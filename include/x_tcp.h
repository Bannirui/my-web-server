//
// Created by dingrui on 1/19/26.
//

#pragma once

#include <string>

class XTcp
{
public:
    // lifecycle
    XTcp()  = default;
    ~XTcp() = default;
    int  CreateSocket();
    bool Bind(uint16_t port);
    bool Connect(const std::string &ip, uint16_t port, int timeout_ms = 1000);
    XTcp Accept() const;
    void Close() const;

    // 负责IO
    /**
     *
     * @param buf
     * @param len the max bytes of buf, exclude 1 space for \0 for c-style string, if the buf is char[1024], len=1023
     * @return receive bytes actually, including \0 char for c-style string
     */
    int Recv(char *buf, int len) const;
    /**
     *
     * @param buf
     * @param len the bytes of buf, including 1 \0 space for c-string, if the buf="ok\n" then len=4
     * @return send bytes actually
     */
    int Send(const char *buf, int len) const;
    /**
     * set socket non-blocked
     * @param block true determines non-blocking
     */
    bool SetBlock(bool block = false) const;

    // socket信息
    int         get_sock() const { return this->m_sock; }
    void        set_sock(int sock_fd) { this->m_sock = sock_fd; }

    // remote
    std::string RemoteIP() const;
    uint16_t    RemotePort() const;

    // local
    std::string LocalIP() const;
    uint16_t    LocalPort() const;

private:
    // TCP的语义是一条已经建立连接的TCP连接 不在成员里面保存ip和port信息是担心不一致
    // ip和port应该是由client和server保存
    int         m_sock{0};
};

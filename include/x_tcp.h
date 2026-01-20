//
// Created by dingrui on 1/19/26.
//

#pragma once

#include <string>

class XTcp
{
public:
    XTcp()  = default;
    ~XTcp() = default;
    int  CreateSocket();
    bool Bind(uint16_t port);
    XTcp Accept();
    void Close();
    /**
     *
     * @param buf
     * @param len the max bytes of buf, exclude 1 space for \0 for c-style string, if the buf is char[1024], len=1023
     * @return receive bytes actually, including \0 char for c-style string
     */
    int Recv(char *buf, int len);
    /**
     *
     * @param buf
     * @param len the bytes of buf, including 1 \0 space for c-string, if the buf="ok\n" then len=4
     * @return send bytes actually
     */
    int  Send(const char *buf, int len);
    bool Connect(const std::string &ip, uint16_t port, int timeout_ms = 1000);
    /**
     * set socket non-blocked
     * @param block true determines non-blocking
     */
    bool SetBlock(bool block = false);

    int         get_sock() const { return this->m_sock; }
    void        set_sock(int sock_fd) { this->m_sock = sock_fd; }
    uint16_t    get_port() const { return this->m_port; }
    std::string get_ip() const { return this->m_ip; }

private:
    int         m_sock{0};
    uint16_t    m_port{0};
    std::string m_ip;
};

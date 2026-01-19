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

    uint32_t    get_sock() const { return this->m_sock; }
    uint16_t    get_port() const { return this->m_port; }
    std::string get_ip() const { return this->m_ip; }

private:
    uint32_t    m_sock{0};
    uint16_t    m_port{0};
    std::string m_ip;
};

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

private:
    int         m_sock{0};
    uint16_t    m_port{0};
    std::string m_ip;
};

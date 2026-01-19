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
    int CreateSocket();

private:
    int            m_sock{0};
    unsigned short m_port{0};
    std::string    m_ip;
};

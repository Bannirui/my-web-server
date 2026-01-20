//
// Created by dingrui on 2026/1/21.
//
#pragma once

#include <memory>

class XTcp;

class XThread
{
public:
    explicit XThread(XTcp &client);
    void Main();

private:
    XTcp &m_client;
};
//
// Created by rui ding on 2026/1/22.
//

#pragma once

#include <string>

#include "x_tcp.h"

struct XHttpRequest;
struct XHttpResponse;
struct XHttpBody;

/**
 * 负责3件事情
 * 1 Serialize Request
 * 2 Send All
 * 3 Parse Response
 * 不需要关心额外的事情 比如HTTP状态语义 Content-Length计算 Header生成
 */
class XHttpClient
{
public:
    /**
     * @param host 要连的目标主机
     * @param port 要连接的目标端口
     */
    bool Connect(const std::string &host, uint16_t port, int timeout_ms = 3000);
    /**
     * 在请求前可能还会对request进行修复 不能加const修饰
     */
    bool Request(XHttpRequest &req, XHttpResponse &resp);
    void Close();

private:
    bool sendAll(const std::string &data);
    bool readResponse(XHttpResponse &resp);
    bool readHeader(std::string &out);
    bool readBody(XHttpBody &body, size_t len);
    void parseHeader(const std::string &header, XHttpResponse &resp);

private:
    // 目标host
    std::string m_host;
    // 目标port
    uint16_t m_port{0};
    XTcp     m_tcp;
};

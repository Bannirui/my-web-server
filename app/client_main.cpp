//
// Created by dingrui on 1/19/26.
//

#include "log/x_log.h"
#include "x_tcp.h"
#include "log/x_dump.h"
#include "http/x_http_client.h"
#include "http/protocol/x_http_request.h"
#include "http/protocol/x_http_response.h"

int main(int argc, char *argv[])
{
    XLog::Init();
    std::string host = "127.0.0.1";
    uint16_t    port = 9527;
    XHttpClient client;
    if (!client.Connect(host, port))
    {
        XLOG_ERROR("connect failed");
        return -1;
    }
    XHttpRequest req;
    req.m_method  = HttpMethod::GET_;
    req.m_uri     = "/index.html";
    req.m_version = "HTTP/1.1";
    req.SetHeader("Host", host + ": " + std::to_string(port));

    XHttpResponse resp;
    if (!client.Request(req, resp))
    {
        XLOG_ERROR("http request failed");
        return -1;
    }
    XLOG_INFO("status={} {}", resp.m_status, resp.m_reason);
    XLOG_INFO("body:\n{}", resp.m_body.m_data);

    client.Close();
    return 0;
}

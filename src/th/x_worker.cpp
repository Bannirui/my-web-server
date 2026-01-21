//
// Created by dingrui on 2026/1/21.
//

#include "th/x_worker.h"

#include <string>
#include <regex>
#include <sys/sendfile.h>
#include <sys/types.h>
#include <unistd.h>

#include "log/x_log.h"
#include "log/x_dump.h"
#include "http/x_http_request.h"
#include "http/x_http_resp.h"
#include "http/x_http_dispatcher.h"

XWorker::XWorker(XTcp client, std::shared_ptr<const XHttpDispatcher> dispatcher)
    : m_client(std::move(client)), m_dispatcher(dispatcher)
{
}

void XWorker::operator()()
{
    // todo how to deal with pretty huge data, more than buf size
    char buf[4096] = {0};
    // recv the client
    int recvLen = this->m_client.Recv(buf, sizeof(buf) - 1);
    if (recvLen <= 0)
    {
        this->m_client.Close();
        return;
    }
    buf[recvLen] = '\0';
    // http协议里面有\r\n转义字符 把不可见的字符转义打印日志
    XLOG_INFO("recv client {} bytes:\n{}", recvLen, DumpBinary(buf, recvLen));
    // for GET request, respond
    // GET /index.html HTTP/1.1\r\n
    // Host: 127.0.0.1:9527\r\n
    // User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:128.0) Gecko/20100101 Firefox/128.0\r\n
    // Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n
    // Accept-Language: en-US,en;q=0.5\r\n
    // Accept-Encoding: gzip, deflate, br, zstd\r\n
    // Connection: keep-alive\r\n
    // Upgrade-Insecure-Requests: 1\r\n
    // Sec-Fetch-Dest: document\r\n
    // Sec-Fetch-Mode: navigate\r\n
    // Sec-Fetch-Site: none\r\n
    // Sec-Fetch-User: ?1\r\n
    // Priority: u=0, i\r\n
    // \r\n
    // the method GET POST PUT OPTION and the url
    std::string src(buf, recvLen);
    std::regex  r("^([A-Z]+) (.+) HTTP/1");
    std::smatch m;
    if (!std::regex_search(src, m, r))
    {
        this->m_client.Close();
        return;
    }
    XHttpRequest req;
    req.raw    = src;
    req.method = ParseMethod(m[1].str());
    req.uri    = m[2].str();
    XLOG_INFO("the client method: {}, uri: {}", m[1].str(), req.uri);

    XHttpResp resp;
    this->m_dispatcher->Dispatch(req, resp);

    // send header
    std::string header = resp.BuildHeader();
    this->m_client.Send(header.data(), header.size());

    // send body or file
    if (resp.m_body.m_type == XHttpBody::Type::Memory)
    {
        this->m_client.Send(resp.m_body.m_data.data(), resp.m_body.m_data.size());
    }
    else if (resp.m_body.m_type == XHttpBody::Type::File)
    {
        off_t offset = 0;
        while (offset < resp.m_body.m_size)
        {
            ssize_t sent = sendfile(m_client.get_sock(), resp.m_body.m_fd, &offset, resp.m_body.m_size - offset);
            if (sent <= 0) break;
        }
        close(resp.m_body.m_fd);
    }
    this->m_client.Close();
}

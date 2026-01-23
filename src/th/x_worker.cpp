//
// Created by dingrui on 2026/1/21.
//

#include "th/x_worker.h"

#include <string>
#include <regex>
#include <utility>
#include <unistd.h>

#include "http/protocol/x_http_request.h"
#include "http/protocol/x_http_response.h"
#include "http/x_http_dispatcher.h"
#include "http/x_http_request_parser.h"
#include "net/x_send_file.h"

XWorker::XWorker(XTcp client, std::shared_ptr<const XHttpDispatcher> dispatcher)
    : m_client(client), m_dispatcher(std::move(dispatcher))
{
}

void XWorker::operator()()
{
    auto clientCloseGuard = [&]() { m_client.Close(); };
    // recv the client
    XHttpRequest       req;
    XHttpRequestParser reqParser;
    // parse request header
    if (!reqParser.ReadRequest(this->m_client, req))
    {
        clientCloseGuard();
        return;
    }
    // parse request line
    XHttpResponse resp;
    this->m_dispatcher->Dispatch(req, resp);

    // send header
    std::string respHeader = resp.BuildHeader();
    this->m_client.Send(respHeader.data(), respHeader.size());

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
            ssize_t sent = SendFile(this->m_client.get_sock(), resp.m_body.m_fd, offset, resp.m_body.m_size - offset);
            if (sent <= 0) break;
        }
        close(resp.m_body.m_fd);
    }
    this->m_client.Close();
}

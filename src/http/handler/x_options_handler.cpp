//
// Created by dingrui on 1/21/26.
//

#include "http/handler/x_options_handler.h"

#include "http/protocol/x_http_response.h"

void XOptionsHandler::Handle(const XHttpRequest &req, XHttpResponse &resp)
{
    // todo
    resp.m_status      = 500;
    resp.m_body.m_type = XHttpBody::Type::Memory;
    resp.m_body.m_data = "<h2>Not Support Options</h1>";
}

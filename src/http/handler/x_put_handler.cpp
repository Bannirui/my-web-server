//
// Created by dingrui on 1/21/26.
//

#include "http/handler/x_put_handler.h"

#include "http/protocol/x_http_request.h"
#include "http/protocol/x_http_resp.h"

void XPutHandler::Handle(const XHttpRequest &req, XHttpResp &resp)
{
    // todo
    resp.m_status      = 500;
    resp.m_body.m_type = XHttpBody::Type::Memory;
    resp.m_body.m_data = "<h2>Not Support Put</h1>";
}

//
// Created by dingrui on 1/21/26.
//

#include "http/handler/x_get_handler.h"

#include "http/protocol/x_http_request.h"
#include "http/protocol/x_http_response.h"

void XGetHandler::Handle(const XHttpRequest &req, XHttpResponse &resp)
{
    this->HttpHandler::respondFile(req.m_uri.m_fileName, resp);
}

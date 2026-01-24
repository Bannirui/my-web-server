//
// Created by dingrui on 1/21/26.
//

#include "http/handler/x_get_handler.h"

#include "http/protocol/x_http_request.h"
#include "http/protocol/x_http_response.h"

void XGetHandler::Handle(const XHttpRequest &req, XHttpResponse &resp)
{
    std::string uri = req.m_uri == "/" ? HTML_URI_INDEX : req.m_uri;
    this->HttpHandler::respondFile(uri, resp);
}

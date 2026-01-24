//
// Created by dingrui on 1/21/26.
//

#include "http/handler/x_options_handler.h"

#include "http/protocol/x_http_response.h"

void XOptionsHandler::Handle(const XHttpRequest &req, XHttpResponse &resp)
{
    this->HttpHandler::respondFile(HTML_URI_NOT_SUPPORT, resp);
}

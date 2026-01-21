//
// Created by dingrui on 1/21/26.
//

#include "http/x_get_handler.h"

#include "http/x_http_request.h"
#include "http/x_http_resp.h"

void XGetHandler::Handle(const XHttpRequest &req, XHttpResp &resp)
{
    resp.body = "<h1>Hello GET</h1>";
}

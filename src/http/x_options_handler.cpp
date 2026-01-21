//
// Created by dingrui on 1/21/26.
//

#include "http/x_options_handler.h"

#include "http/x_http_request.h"
#include "http/x_http_resp.h"

void XOptionsHandler::Handle(const XHttpRequest &req, XHttpResp &resp)
{
    // todo
    resp.contentLength = 0;
}

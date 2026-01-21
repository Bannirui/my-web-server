//
// Created by dingrui on 1/21/26.
//

#pragma once
#include "x_http_handler.h"

class XGetHandler : public IHttpHandler
{
public:
    void Handle(const XHttpRequest &req, XHttpResp &resp) override;
};

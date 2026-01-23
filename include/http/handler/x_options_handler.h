//
// Created by dingrui on 1/21/26.
//

#pragma once
#include "x_http_handler.h"

class XOptionsHandler : public IHttpHandler
{
public:
    void Handle(const XHttpRequest &req, XHttpResponse &resp) override;
};

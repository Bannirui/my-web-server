//
// Created by dingrui on 1/21/26.
//

#pragma once

struct XHttpRequest;
struct XHttpResp;

class IHttpHandler
{
public:
    virtual ~IHttpHandler()                                       = default;
    virtual void Handle(const XHttpRequest &req, XHttpResp &resp) = 0;

protected:
    IHttpHandler() = default;
};

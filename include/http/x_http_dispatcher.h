//
// Created by dingrui on 1/21/26.
//

#pragma once

#include <memory>
#include <unordered_map>

#include "x_http_handler.h"

enum class HttpMethod;
struct XHttpRequest;
struct XHttpResp;

class XHttpDispatcher
{
public:
    void Register(HttpMethod method, std::unique_ptr<IHttpHandler> handler);
    void Dispatch(const XHttpRequest &req, XHttpResp &resp) const;

private:
    std::unordered_map<HttpMethod, std::unique_ptr<IHttpHandler>> m_handlers;
};

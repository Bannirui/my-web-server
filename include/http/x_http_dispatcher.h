//
// Created by dingrui on 1/21/26.
//

#pragma once

#include <memory>
#include <unordered_map>

#include "handler/x_http_handler.h"

enum class HttpMethod;
struct XHttpRequest;
struct XHttpResponse;

class XHttpDispatcher
{
public:
    void Register(HttpMethod method, std::unique_ptr<HttpHandler> handler);
    void Dispatch(const XHttpRequest &req, XHttpResponse &resp) const;

private:
    std::unordered_map<HttpMethod, std::unique_ptr<HttpHandler>> m_handlers;
};

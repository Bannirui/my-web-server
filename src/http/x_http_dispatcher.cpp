//
// Created by dingrui on 1/21/26.
//

#include "http/x_http_dispatcher.h"

#include "http/x_http_handler.h"
#include "http/x_http_request.h"
#include "http/x_http_resp.h"

void XHttpDispatcher::Register(HttpMethod method, std::unique_ptr<IHttpHandler> handler)
{
    this->m_handlers[method] = std::move(handler);
}

void XHttpDispatcher::Dispatch(const XHttpRequest &req, XHttpResp &resp) const
{
    auto it = this->m_handlers.find(req.m_method);
    if (it == this->m_handlers.end())
    {
        resp.m_status      = 405;
        resp.m_contentType = CONTENT_TYPE_HTML;
        resp.m_body.m_type = XHttpBody::Type::Memory;
        resp.m_body.m_data = "<h2>Method Not Allowed</h1>";
        return;
    }
    it->second->Handle(req, resp);
}

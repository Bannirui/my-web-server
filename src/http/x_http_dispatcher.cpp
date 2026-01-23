//
// Created by dingrui on 1/21/26.
//

#include "http/x_http_dispatcher.h"

#include "http/handler/x_http_handler.h"
#include "http/protocol/x_http_request.h"
#include "http/protocol/x_http_response.h"

void XHttpDispatcher::Register(HttpMethod method, std::unique_ptr<IHttpHandler> handler)
{
    this->m_handlers[method] = std::move(handler);
}

void XHttpDispatcher::Dispatch(const XHttpRequest &req, XHttpResponse &resp) const
{
    auto it = this->m_handlers.find(req.m_method);
    if (it == this->m_handlers.end())
    {
        resp.m_status      = 405;
        // todo
        // resp.m_contentType = CONTENT_TYPE_HTML;
        resp.m_body.m_type = XHttpBody::Type::Memory;
        resp.m_body.m_data = "<h2>Method Not Allowed</h1>";
        return;
    }
    it->second->Handle(req, resp);
}

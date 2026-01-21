//
// Created by dingrui on 1/21/26.
//

#include "http/x_get_handler.h"

#include <fcntl.h>

#include "http/x_http_request.h"
#include "http/x_http_resp.h"
#include "log/x_log.h"

#include <sys/stat.h>

void XGetHandler::Handle(const XHttpRequest &req, XHttpResp &resp)
{
    std::string uri      = req.uri == "/" ? "index.html" : req.uri;
    std::string filePath = "www" + uri;
    int         fd       = open(filePath.c_str(), O_RDONLY);
    if (fd < 0)
    {
        resp.m_status      = 404;
        resp.m_contentType = CONTENT_TYPE_HTML;
        resp.m_body.m_type = XHttpBody::Type::Memory;
        resp.m_body.m_data = "<h2>Not Found</h1>";
        return;
    }

    struct stat st{};
    fstat(fd, &st);

    resp.m_status      = 200;
    resp.m_contentType = CONTENT_TYPE_HTML;
    resp.m_body.m_type = XHttpBody::Type::File;
    resp.m_body.m_fd   = fd;
    resp.m_body.m_size = st.st_size;
}

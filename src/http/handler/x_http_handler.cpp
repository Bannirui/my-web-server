//
// Created by dingrui on 1/24/26.
//

#include "http/handler/x_http_handler.h"

#include <fcntl.h>
#include <sys/stat.h>

#include "http/protocol/x_http_request.h"
#include "http/protocol/x_http_response.h"

void HttpHandler::respondFile(const std::string &fileName, XHttpResponse &resp) const
{
    std::string filePath = HTML_FILE_DIRECTORY + fileName;
    int         fd       = open(filePath.c_str(), O_RDONLY);
    if (fd < 0)
    {
        resp.m_status      = 404;
        resp.m_body.m_type = XHttpBody::Type::Memory;
        resp.m_body.m_data = "<h2>Not Found</h2>";
        return;
    }
    struct stat st{};
    fstat(fd, &st);

    resp.m_status      = 200;
    resp.m_body.m_type = XHttpBody::Type::File;
    resp.m_body.m_fd   = fd;
    resp.m_body.m_size = st.st_size;
}

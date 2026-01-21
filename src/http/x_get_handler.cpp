//
// Created by dingrui on 1/21/26.
//

#include "http/x_get_handler.h"

#include "http/x_http_request.h"
#include "http/x_http_resp.h"
#include "log/x_log.h"

void XGetHandler::Handle(const XHttpRequest &req, XHttpResp &resp)
{
    std::string fileName = req.uri;
    if ("/" == fileName)
    {
        fileName = "/index.html";
    }
    std::string filePath = "www";
    filePath.append(fileName);
    FILE *fp = fopen(filePath.c_str(), "rb");
    if (fp == nullptr)
    {
        resp.body = "<h1>Page Not Found</h1>";
    }
    // fseek(fp, 0, SEEK_END);
    // todo int determins the file size max
    // int fileSize = ftell(fp);
    // fseek(fp, 0, SEEK_SET);
    // read the html file
    char        buf[4096] = {0};
    int         len       = fread(buf, 1, sizeof(buf), fp);
    std::string body(buf, len);
    XLOG_INFO("the html path is {}:{}", filePath, body);
    resp.body = body;
}

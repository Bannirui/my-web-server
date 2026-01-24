//
// Created by dingrui on 1/21/26.
//

#pragma once

#include <string>

#define HTML_FILE_DIRECTORY "www/"
#define HTML_URI_INDEX "index.html"
#define HTML_URI_404 "404.html"
#define HTML_URI_NOT_SUPPORT "not_support.html"

struct XHttpRequest;
struct XHttpResponse;

class HttpHandler
{
public:
    virtual ~HttpHandler()                                            = default;
    virtual void Handle(const XHttpRequest &req, XHttpResponse &resp) = 0;

protected:
    HttpHandler() = default;

    /**
     * @param fileName html file, parse content for reqeust, a sample is '404.html'
     */
    virtual void respondFile(const std::string &fileName, XHttpResponse &resp) const;
};

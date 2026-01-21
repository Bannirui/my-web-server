//
// Created by dingrui on 1/21/26.
//

#pragma once

#include <string>

struct XHttpResp
{
    int         status = 200;
    std::string body;
    std::string contentType = "text/html";

    std::string Serialize() const
    {
        std::string resp;
        resp.append("HTTP/1.1 " + std::to_string(status) + " OK\r\n");
        resp.append("Server: XHTTP\r\n");
        resp.append("Content-Type: " + contentType + "\r\n");
        resp.append("Content-Length: " + std::to_string(body.size()) + "\r\n");
        resp.append("\r\n");
        resp.append(body);
        return resp;
    }
};

//
// Created by dingrui on 1/22/26.
//

#pragma once

#include <string>

class XTcp;
class XHttpRequest;

class XHttpRequestParser
{
public:
    bool ReadRequest(XTcp &client, XHttpRequest &req);

private:
    bool readHeader(XTcp &client, std::string &out);
    bool readBody(XTcp &client, std::string &body, size_t len);
    bool parseHeader(const std::string &header, XHttpRequest &req);
};

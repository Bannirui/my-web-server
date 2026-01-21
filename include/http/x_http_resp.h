//
// Created by dingrui on 1/21/26.
//

#pragma once

#include <string>

#define CONTENT_TYPE_HTML "text/html"
#define CONTENT_TYPE_PLAIN "text/plain"

struct XHttpResp
{
    int         status      = 200;
    std::string contentType = CONTENT_TYPE_HTML;

    // for small response, like errors hint
    std::string body;

    // for file response, like static html
    int    fileFd        = -1;
    bool   isFile        = false;
    off_t contentLength = 0;

    bool HasBody() const { return !body.empty(); }

    std::string SerializeHeader() const
    {
        std::string header;
        header.append("HTTP/1.1 " + std::to_string(status) + " OK\r\n");
        header.append("Server: XHTTP\r\n");
        header.append("Content-Type: " + contentType + "\r\n");
        header.append("Content-Length: " + std::to_string(contentLength) + "\r\n");
        header.append("\r\n");
        return header;
    }

    std::string BuildHeader(bool keep_alive) const
    {
        size_t len = isFile ? contentLength : body.size();
        return BuildHeader(status, contentType, len, keep_alive);
    }

    inline std::string BuildHeader(int stat, std::string_view content_type, size_t content_len,
                                   bool keep_alive = false) const
    {
        std::string h;
        h.reserve(256);  // avoid reallocs
        h.append("HTTP/1.1 " + std::to_string(stat) + " " + ReasonPhrase(stat) + "\r\n");
        h.append("Server: XHTTP\r\n");
        if (!contentType.empty())
        {
            h.append("Content-Type: ");
            h += content_type;
            h.append("\r\n");
        }

        h.append("Content-Length: " + std::to_string(content_len) + "\r\n");

        h.append("Connection: ");
        h += keep_alive ? "keep-alive" : "close";
        h.append("\r\n");

        h.append("\r\n");  // end of headers
        return h;
    }

    inline const char *ReasonPhrase(int stat) const
    {
        switch (stat)
        {
            case 200:
                return "OK";
            case 400:
                return "Bad Request";
            case 403:
                return "Forbidden";
            case 404:
                return "Not Found";
            case 405:
                return "Method Not Allowed";
            case 500:
                return "Internal Server Error";
            default:
                return "Unknown";
        }
    }
};

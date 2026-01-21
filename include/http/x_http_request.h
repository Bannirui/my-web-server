//
// Created by dingrui on 1/21/26.
//

#pragma once

#include <string>
#include <string_view>

enum class HttpMethod
{
    GET_,
    POST_,
    PUT_,
    DELETE_,
    OPTIONS_,
    UNKNOWN_
};

inline HttpMethod ParseMethod(std::string_view method)
{
    if (method == "GET") return HttpMethod::GET_;
    if (method == "POST") return HttpMethod::POST_;
    if (method == "PUT") return HttpMethod::PUT_;
    if (method == "DELETE") return HttpMethod::DELETE_;
    if (method == "OPTIONS") return HttpMethod::OPTIONS_;
    return HttpMethod::UNKNOWN_;
}

struct XHttpRequest
{
    HttpMethod  method;
    std::string uri;
    std::string raw;
};

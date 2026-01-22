//
// Created by rui ding on 2026/1/22.
//

#pragma once

#include <string>

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

inline std::string_view MethodToString(HttpMethod m)
{
    switch (m)
    {
        case HttpMethod::GET_:
            return "GET";
        case HttpMethod::POST_:
            return "POST";
        case HttpMethod::PUT_:
            return "PUT";
        case HttpMethod::DELETE_:
            return "DELETE";
        case HttpMethod::OPTIONS_:
            return "OPTIONS";
        default:
            return "UNKNOWN";
    }
}
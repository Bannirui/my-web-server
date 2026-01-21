//
// Created by dingrui on 1/22/26.
//

#pragma once

#include <string>
#include <string_view>
#include <cctype>

inline void Trim(std::string &s)
{
    size_t begin = 0;
    size_t end   = s.size();
    // trim left
    while (begin < end && std::isspace(static_cast<unsigned char>(s[begin])))
    {
        ++begin;
    }
    // trim right
    while (end > begin && std::isspace(static_cast<unsigned char>(s[end - 1])))
    {
        --end;
    }
    if (begin == 0 && end == s.size())
    {
        return;
    }
    s = s.substr(begin, end - begin);
}

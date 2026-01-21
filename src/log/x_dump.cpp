//
// Created by rui ding on 2026/1/21.
//

#include "log/x_dump.h"

std::string DumpBinary(const void *data, size_t len)
{
    const unsigned char *p = static_cast<const unsigned char *>(data);
    std::string          out;
    out.reserve(len << 1);
    for (size_t i = 0; i < len; ++i)
    {
        unsigned char c = p[i];
        if (c == '\r')
        {
            out += "\\r";
        }
        else if (c == '\n')
        {
            out += "\\n\n";
        }
        else if (std::isprint(c))
        {
            out += c;
        }
        else
        {
            char tmp[8];
            std::snprintf(tmp, sizeof(tmp), "\\x%02X", c);
            out += tmp;
        }
    }
    return out;
}

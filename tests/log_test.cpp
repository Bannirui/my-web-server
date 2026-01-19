//
// Created by dingrui on 1/19/26.
//

#include "x_log.h"

int main()
{
    XLog::Init();
    XLOG_INFO("info, hello world");
    XLOG_WARN("warn, hello world");
    XLOG_ERROR("error, hello world");
    return 0;
}
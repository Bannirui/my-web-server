//
// Created by dingrui on 1/21/26.
//

#include "x_log.h"

int main()
{
    XLog::Init();
    char arr[1024] = {0};
    for (int i = 0; i < 1024; i++)
    {
        arr[i] = 'a';
    }
    arr[0]    = '\0';
    arr[1024] = '\0';
    XLOG_INFO("{}", std::string(arr, 1023));
    return 0;
}
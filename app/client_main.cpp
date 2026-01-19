//
// Created by dingrui on 1/19/26.
//

#include "x_log.h"
#include "x_tcp.h"

int main(int argc, char *argv[])
{
    XLog::Init();

    XTcp client;
    client.Connect("127.0.0.1", 9527);

    return 0;
}

//
// Created by rui ding on 2025/9/11.
//

#include "web_server.h"
#include "log/log.h"

int main()
{
    WebServer server(9527, true);
    server.run();
    return 0;
}
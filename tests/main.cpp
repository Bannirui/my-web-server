//
// Created by rui ding on 2025/9/11.
//

#include "web_server.h"

int main()
{
    WebServer server;
    server.listen();
    server.run();
    return 0;
}
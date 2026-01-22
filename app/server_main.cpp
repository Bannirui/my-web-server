//
// Created by dingrui on 1/19/26.
//

#include "log/x_log.h"
#include "x_tcp.h"
#include "th/x_worker.h"
#include "th/x_thread_pool.h"
#include "x_poll_factory.h"
#include "http/handler/x_delete_handler.h"
#include "http/handler/x_get_handler.h"
#include "http/x_http_dispatcher.h"
#include "http/x_http_server.h"
#include "http/protocol/x_http_request.h"
#include "http/handler/x_options_handler.h"
#include "http/handler/x_post_handler.h"
#include "http/handler/x_put_handler.h"

// 线程池
XThreadPool pool(std::thread::hardware_concurrency());

int main(int argc, char *argv[])
{
    XLog::Init();

    uint16_t port = 9527;
    if (argc > 1)
    {
        port = atoi(argv[1]);
    }

    auto dispatcher = std::make_shared<XHttpDispatcher>();
    dispatcher->Register(HttpMethod::GET_, std::make_unique<XGetHandler>());
    dispatcher->Register(HttpMethod::POST_, std::make_unique<XPostHandler>());
    dispatcher->Register(HttpMethod::PUT_, std::make_unique<XPutHandler>());
    dispatcher->Register(HttpMethod::DELETE_, std::make_unique<XDeleteHandler>());
    dispatcher->Register(HttpMethod::OPTIONS_, std::make_unique<XOptionsHandler>());

    XHttpServer server(port);
    server.SetDispatcher(dispatcher);
    if (!server.Init())
    {
        XLOG_ERROR("http server init failed");
        return -1;
    }
    server.Run();
    return 0;
}
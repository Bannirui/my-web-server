//
// Created by rui ding on 2025/9/12.
//

#include <spdlog/sinks/stdout_color_sinks-inl.h>

#include "log/log.h"

std::shared_ptr<spdlog::logger> MyLog::logger;

void MyLog::Init()
{
    spdlog::set_pattern("%^[%T] %n: %v%$");
    logger = spdlog::stdout_color_mt("MyWebServer");
    logger->set_level(spdlog::level::trace);
}

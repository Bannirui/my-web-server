//
// Created by rui ding on 2025/9/12.
//

#include "log.h"

std::shared_ptr<spdlog::logger> my_ws::Log::logger;

void my_ws::Log::Init()
{
    spdlog::set_pattern("%^[%T] %n: %v%$");
    logger = spdlog::stdout_color_mt("MyWebServer");
    logger->set_level(spdlog::level::trace);
}

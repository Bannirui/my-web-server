//
// Created by rui ding on 2025/9/12.
//

#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks-inl.h>

#define XLOG_INFO(...) XLog::get_logger()->info(__VA_ARGS__)
#define XLOG_WARN(...) XLog::get_logger()->warn(__VA_ARGS__)
#define XLOG_ERROR(...) XLog::get_logger()->error(__VA_ARGS__)

class XLog
{
public:
    static void                                   Init();
    inline static std::shared_ptr<spdlog::logger> get_logger()
    {
        if (!logger)
        {
            // 兜底 保证不会崩溃
            spdlog::set_pattern("%^[%T] %n: %v%$");
            logger = spdlog::stdout_color_mt("MyWebServer");
            logger->set_level(spdlog::level::trace);
            spdlog::warn("MyLog::Init() was not called explicitly, using default logger.");
        }
        return logger;
    }

private:
    static std::shared_ptr<spdlog::logger> logger;
};
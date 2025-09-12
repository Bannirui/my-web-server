//
// Created by rui ding on 2025/9/12.
//

#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks-inl.h>

#define MY_LOG_INFO(...) MyLog::GetLogger()->info(__VA_ARGS__)
#define MY_LOG_ERROR(...) MyLog::GetLogger()->error(__VA_ARGS__)

class MyLog
{
public:
    static void                                   Init();
    inline static std::shared_ptr<spdlog::logger> GetLogger()
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
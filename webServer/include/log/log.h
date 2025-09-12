//
// Created by rui ding on 2025/9/12.
//

#pragma once

#include <spdlog/spdlog.h>

#define MY_LOG_INFO(...) MyLog::GetLogger()->info(__VA_ARGS__)

class MyLog
{
public:
    static void                                   Init();
    inline static std::shared_ptr<spdlog::logger> GetLogger() { return logger; }

private:
    static std::shared_ptr<spdlog::logger> logger;
};
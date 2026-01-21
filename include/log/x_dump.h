//
// Created by rui ding on 2026/1/21.
//

#pragma once

#include <string>

/**
 * 字符串里面会有不可见字符 进行转义
 */
std::string DumpBinary(const void *data, size_t len);
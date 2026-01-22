//
// Created by rui ding on 2026/1/22.
//

#pragma once

#include <sys/types.h>

// 返回值语义统一：
// >0 实际发送字节数
//  0 未发送
// <0 错误
/**
 * linux跟mac上sendfile这个系统调用语义不一样 做个适配
 * file发向socket
 * @param sock_fd 接收方
 * @param file_fd 发送方
 * @param offset 从哪儿开始发
 * @param file_size 文件总大小 file_size-offset就是发多少
 * @return >0表示实际发送字节数 0表示未发送 <0表示错误
 */
ssize_t SendFile(int sock_fd, int file_fd, off_t &offset, size_t file_size);

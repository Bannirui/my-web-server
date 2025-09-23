//
// Created by rui ding on 2025/9/23.
//

#pragma once

/**
 * 根据状态转移 通过主从状态机封装了http连接类
 * 主状态机在内部调用从状态机 从状态机将处理状态和数据传给主状态机
 * 1 客户端发出http连接请求
 * 2 从状态机读取数据 更新自身状态和接收数据 传给主状态机
 * 3 主状态机根据从状态机状态 更新自身状态 决定响应请求还是继续读取
 */
class my_http
{
public:
    // 封装的客户端连接计数 每来一个tcp连接就封装一个 上限是系统socket fd的限制
    static int s_user_count;
};

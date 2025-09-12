//
// Created by rui ding on 2025/9/12.
//

#pragma once

class FdUtil
{
public:
    /**
     * fd设置非阻塞
     * @return 原来的属性
     */
    static int setNonBlocking(int fd);
};

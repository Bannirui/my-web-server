//
// Created by rui ding on 2025/9/12.
//

#include <sys/fcntl.h>

#include "util/fd_util.h"

int FdUtil::setNonBlocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

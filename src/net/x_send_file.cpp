//
// Created by rui ding on 2026/1/22.
//

#include "net/x_send_file.h"

#include <errno.h>

#if defined(__APPLE__)
#include <sys/socket.h>
#include <sys/uio.h>
#elif defined(__linux__)
#include <sys/sendfile.h>
#endif

ssize_t SendFile(int sock_fd, int file_fd, off_t &offset, size_t file_size)
{
#if defined(__APPLE__)
    // 要发多少
    off_t len = file_size - offset;
    int   ret = sendfile(file_fd, sock_fd, offset, &len, nullptr, 0);
    if (ret == 0 || errno == EAGAIN || errno == EWOULDBLOCK)
    {
        offset += len;
        return len;
    }
    return -1;
#elif defined(__linux__)
    ssize_t sent = sendfile(sock_fd, file_fd, &offset, file_size - offset);
    if (sent > 0) offset += sent;
    return sent;
#endif
}

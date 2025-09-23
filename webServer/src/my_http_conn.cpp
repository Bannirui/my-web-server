//
// Created by rui ding on 2025/9/23.
//

#include <sys/event.h>

#include "my_http_conn.h"

int MyHttpConn::s_user_count = 0;
MyHttpConn::MyHttpConn() {}
MyHttpConn::~MyHttpConn() {}
void MyHttpConn::Init(int sockfd, const sockaddr_in& addr, MyKqueue* selector)
{
    _serverSockFd  = sockfd;
    _clientAddress = addr;
    _selector      = selector;
    init();
    // 更新连接计数
    s_user_count++;
}
void MyHttpConn::init() {}
void MyHttpConn::Close()
{
    this->_selector->RemoveEvent(this->_serverSockFd, EVFILT_READ | EVFILT_WRITE);
    close(this->_serverSockFd);
    s_user_count--;
}

//
// Created by rui ding on 2025/9/23.
//

#include "util/timer.h"
Timer::Timer() {}
Timer::Timer(time_t expire, MyHttpConn* conn) : _expire(expire), _conn(conn) {}
Timer::~Timer() {}
void Timer::Delete()
{
    // todo
}

TimerLst::TimerLst() {}
TimerLst::~TimerLst() {}
void TimerLst::Del(Timer* timer)
{
    // todo
}

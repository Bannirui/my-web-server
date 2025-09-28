//
// Created by rui ding on 2025/9/23.
//

#include "util/timer.h"
Timer::Timer() {}
Timer::Timer(time_t expire, MyHttpConn* conn) : _expire(expire), _conn(conn) {}
Timer::~Timer() {}
void Timer::SetExpire(time_t expire)
{
    _expire = expire;
}
void Timer::Delete()
{
    // todo
}
time_t Timer::GetExpire()
{
    return _expire;
}

TimerLst::TimerLst() {}
TimerLst::~TimerLst() {}
void TimerLst::Del(Timer* timer)
{
    // todo
}
void TimerLst::Reset(Timer* timer)
{
    if (!timer) return;
    Timer* next = timer->_Next;
    if (!next || timer->GetExpire() <= next->GetExpire())
    {
        // 相对升序没变 维持现状
        return;
    }
    if (timer == _Head)
    {
        // 头摘掉重新顺序添加
        _Head        = _Head->_Next;
        _Head->_Prev = nullptr;
        timer->_Next = NULL;
        addTimer(timer);
    }
    else
    {
        // 摘掉重新顺序添加
        timer->_Prev->_Next = timer->_Next;
        timer->_Next->_Prev = timer->_Prev;
        addTimer(timer->_Next, timer);
    }
}
void TimerLst::addTimer(Timer* timer)
{
    addTimer(_Head, timer);
}
void TimerLst::addTimer(Timer* node, Timer* timer)
{
    Timer* prev = node;
    Timer* cur  = prev->_Next;
    while (cur)
    {
        if (timer->GetExpire() < cur->GetExpire())
        {
            // timer就放在这
            prev->_Next  = timer;
            timer->_Next = cur;
            cur->_Prev   = timer;
            timer->_Prev = prev;
            break;
        }
        // 往后冒泡找合适的位置
        prev = cur;
        cur  = cur->_Next;
    }
    if (!cur)
    {
        // 整个链条都被找过了一遍说明新加的节点被安置到最后 尾指针要更新
        prev->_Next  = timer;
        timer->_Prev = prev;
        timer->_Next = NULL;
        _Tail        = timer;
    }
}

#pragma once

#include "Timer.h"
#include "Channel.h"
#include <set>
#include <vector>

class EventLoop;

class TimerQueue {
public:
    typedef std::pair<Timestamp, Timer *> Entry;
    typedef std::set<Entry> TimerList;

    explicit TimerQueue(EventLoop *loop);
    ~TimerQueue();

    void addTimer(TimerCallback cb, Timestamp when);

    std::vector<Entry> getExpired(Timestamp now);

private:
    void addTimerInLoop(Timer *timer);
    bool insert(Timer* timer);
    void handleRead();


    EventLoop *loop_;
    const int timerfd_;
    Channel timerChannel_;
    TimerList timers_;
};

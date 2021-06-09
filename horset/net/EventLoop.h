#pragma once

#include <vector>
#include <functional>
#include <iostream>
#include "../base/CurrentThread.h"
#include "../base/Timestamp.h"

class Poller;
class Channel;
class TimerQueue;

class EventLoop {
public:
    typedef std::function<void()> Functor;
    typedef std::function<void()> TimerCallback;

    EventLoop();

    void loop();

    void updateChannel(Channel *channel);

    void quit() { quit_ = true; }

    void runInLoop(Functor cb);

    void runAt(Timestamp time, TimerCallback cb);

    void runAfter(double delay, TimerCallback cb);

    void assertInLoopThread() {
        if (!isInLoopThread()) {
            std::cerr << "loop not in io thread" << std::endl;
        }
    }

private:
    typedef std::vector<Channel *> ChannelList;

    bool isInLoopThread() { return CurrentThread::tid() == tid; }

    int tid;
    Poller *poller_;
    TimerQueue *timerQueue_;
    ChannelList activeChannels;
    bool quit_ = false;
};

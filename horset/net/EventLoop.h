#pragma once

#include <vector>
#include <functional>
#include <iostream>
#include "horset/base/CurrentThread.h"
#include "horset/base/Timestamp.h"
#include "horset/base/Mutex.h"

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

    void quit();

    void runInLoop(Functor cb);

    void runAt(Timestamp time, TimerCallback cb);

    void runAfter(double delay, TimerCallback cb);

    void assertInLoopThread() {
        if (!isInLoopThread()) {
            std::cerr << "loop not in io thread" << std::endl;
        }
    }

    void wakeup();

private:
    typedef std::vector<Channel *> ChannelList;

    bool isInLoopThread() { return CurrentThread::tid() == tid; }

    void queueInLoop(Functor cb);

    void handleRead();
    void doPendingFunctors();

    int tid;
    bool callingPendingFunctors_;
    Poller *poller_;
    TimerQueue *timerQueue_;
    int wakeupFd_;
    Channel *wakeupChannel_;
    ChannelList activeChannels;
    bool quit_ = false;
    MutexLock mutex_;
    std::vector<Functor> pendingFunctors_;

};

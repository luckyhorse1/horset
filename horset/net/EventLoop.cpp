#include "EventLoop.h"
#include "poller/PollPoller.h"
#include "horset/base/CurrentThread.h"
#include "TimerQueue.h"

__thread EventLoop *self = NULL;

EventLoop::EventLoop() : tid(CurrentThread::tid()), poller_(Poller::newDefaultPoller()),
                         timerQueue_(new TimerQueue(this)) {
    if (self) {
        std::cerr << "already has a event loop" << std::endl;
    } else {
        self = this;
    }
}

void EventLoop::loop() {
    assertInLoopThread();
    quit_ = false;
    while (!quit_) {
        activeChannels.clear();
        poller_->poll(&activeChannels);
        for (auto it: activeChannels) {
            it->handleEvent();
        }
    }
}

void EventLoop::updateChannel(Channel *channel) {
    poller_->updateChannel(channel);
}

void EventLoop::runInLoop(Functor cb) {
    if (isInLoopThread()) {
        cb();
    }
}

void EventLoop::runAt(Timestamp time, TimerCallback cb) {
    return timerQueue_->addTimer(std::move(cb), time);
}

void EventLoop::runAfter(double delay, TimerCallback cb) {
    Timestamp time(addTime(Timestamp::now(), delay));
    return runAt(time, std::move(cb));
}


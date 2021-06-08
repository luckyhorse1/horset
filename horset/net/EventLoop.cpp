#include "EventLoop.h"
#include "poller/PollPoller.h"
#include "horset/base/CurrentThread.h"
#include <iostream>

__thread EventLoop *self = NULL;

EventLoop::EventLoop() : tid(CurrentThread::tid()), poller_(Poller::newDefaultPoller()) {
    if (self) {
        std::cerr << "already has a event loop" << std::endl;
    } else {
        self = this;
    }
}

void EventLoop::loop() {
    if (CurrentThread::tid() != tid) {
        std::cerr << "loop not in io thread" << std::endl;
    }
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


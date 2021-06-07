#include "EventLoop.h"
#include "horset/base/CurrentThread.h"
#include <iostream>

__thread EventLoop* self = NULL;
EventLoop::EventLoop():tid(CurrentThread::tid()) {
    if (self) {
        std::cerr<<"already has a event loop"<<std::endl;
    } else {
        self = this;
    }
}

void EventLoop::loop() {
    if (CurrentThread::tid() != tid) {
        std::cerr<<"loop not in io thread"<<std::endl;
    }
}


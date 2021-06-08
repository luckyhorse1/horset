//
// Created by xiaoma on 6/8/21.
//

#pragma once

#include <functional>
#include "EventLoop.h"

class Channel {
public:
    typedef std::function<void()> ReadEventCallback;

    Channel(EventLoop* loop, int fd);

    int fd() { return fd_; }

    int events() { return events_; }

    void setRevents(int revents) { revents_ = revents; }

    void enableReading() {
        events_ |= kReadEvent;
        update();
    }

    bool isNoneEvent() {return events_ == kNoneEvent; }

    void setReadCallback(ReadEventCallback cb) { readCallback_ = std::move(cb); }

    void handleEvent();

    int index() { return index_; }

    void setIndex(int index) { index_ = index; }

private:
    void update();

    static const int kReadEvent;
    static const int kNoneEvent;

    EventLoop* loop_;
    int fd_;
    int events_;
    int revents_;
    int index_;
    ReadEventCallback readCallback_;
};



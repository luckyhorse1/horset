#pragma once

#include <functional>
#include "../base/Timestamp.h"

typedef std::function<void()> TimerCallback;

class Timer {
public:
    Timer(TimerCallback cb, Timestamp when) : callback_(std::move(cb)), expiration_(when) {}

    void run() const {
        callback_();
    }

    Timestamp expiration() { return expiration_; }

private:
    TimerCallback callback_;
    Timestamp expiration_;
};

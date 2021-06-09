//
// Created by xiaoma on 6/9/21.
//

#include "TimerQueue.h"
#include <sys/timerfd.h>
#include <iostream>
#include <cstring> // memset
#include <assert.h>

namespace {
    int createTimerfd() {
        int timerfd = ::timerfd_create(CLOCK_MONOTONIC,
                                       TFD_NONBLOCK | TFD_CLOEXEC);
        if (timerfd < 0) {
            std::cerr << "Failed in timerfd_create" << std::endl;
        }
        return timerfd;
    }

    struct timespec howMuchTimeFromNow(Timestamp when) {
        int64_t microseconds = when.microSecondsSinceEpoch()
                               - Timestamp::now().microSecondsSinceEpoch();
        if (microseconds < 100) {
            microseconds = 100;
        }
        struct timespec ts;
        ts.tv_sec = static_cast<time_t>(
                microseconds / Timestamp::kMicroSecondsPerSecond);
        ts.tv_nsec = static_cast<long>(
                (microseconds % Timestamp::kMicroSecondsPerSecond) * 1000);
        return ts;
    }

    void resetTimerfd(int timerfd, Timestamp expiration) {
        struct itimerspec newValue;
        struct itimerspec oldValue;
        std::memset(&newValue, 0, sizeof newValue);
        std::memset(&oldValue, 0, sizeof oldValue);
        newValue.it_value = howMuchTimeFromNow(expiration);
        int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
        if (ret) {
            std::cerr << "timerfd_settime()" << std::endl;
        }
    }

    void readTimerfd(int timerfd) {
        uint64_t howmany;
        ssize_t n = ::read(timerfd, &howmany, sizeof howmany);
        if (n != sizeof howmany) {
            std::cerr << "TimerQueue::handleRead() reads " << n << " bytes instead of 8" << std::endl;
        }
    }

}

TimerQueue::TimerQueue(EventLoop *loop) : loop_(loop), timerfd_(createTimerfd()), timerChannel_(loop_, timerfd_) {
    timerChannel_.setReadCallback([this] { handleRead(); });
    timerChannel_.enableReading();
}

TimerQueue::~TimerQueue() {
    ::close(timerfd_);
}

void TimerQueue::addTimer(TimerCallback cb, Timestamp when) {
    auto timer = new Timer(std::move(cb), when);
    loop_->runInLoop([this, timer] { addTimerInLoop(timer); });
}

std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now) {
    std::vector<Entry> expired;
    Entry sentry(now, reinterpret_cast<Timer *>(UINTPTR_MAX));
    auto end = timers_.lower_bound(sentry);
    assert(end == timers_.end() || now < end->first);
    std::copy(timers_.begin(), end, back_inserter(expired));
    return expired;
}

void TimerQueue::addTimerInLoop(Timer *timer) {
    loop_->assertInLoopThread();
    bool isEarly = insert(timer);
    if (isEarly) {
        resetTimerfd(timerfd_, timer->expiration());
    }
}

bool TimerQueue::insert(Timer *timer) {
    auto when = timer->expiration();
    bool isEarly = false;
    if (timers_.empty() || when < timers_.begin()->first) {
        isEarly = true;
    }
    timers_.insert(Entry(timer->expiration(), timer));
    return isEarly;
}

void TimerQueue::handleRead() {
    loop_->assertInLoopThread();
    Timestamp now(Timestamp::now());
    readTimerfd(timerfd_);

    std::vector<Entry> expired = getExpired(now);

    for (const Entry &it : expired) {
        it.second->run();
    }
}
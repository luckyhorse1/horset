#include "EventLoop.h"
#include "poller/PollPoller.h"
#include "horset/base/CurrentThread.h"
#include "TimerQueue.h"
#include <sys/eventfd.h>

__thread EventLoop *self = NULL;
namespace {
    int createEventfd() {
        int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
        if (evtfd < 0) {
            std::cerr << "Failed in eventfd" << std::endl;
            abort();
        }
        return evtfd;
    }

}

EventLoop::EventLoop() : tid(CurrentThread::tid()), callingPendingFunctors_(false), poller_(Poller::newDefaultPoller()),
                         timerQueue_(new TimerQueue(this)), wakeupFd_(createEventfd()),
                         wakeupChannel_(new Channel(this, wakeupFd_)) {
    if (self) {
        std::cerr << "already has a event loop" << std::endl;
    } else {
        self = this;
    }
    wakeupChannel_->setReadCallback([this] { handleRead(); });
    wakeupChannel_->enableReading();
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
        doPendingFunctors();
    }
}

void EventLoop::updateChannel(Channel *channel) {
    poller_->updateChannel(channel);
}

void EventLoop::quit() {
    quit_ = true;
    if (!isInLoopThread()) {
        wakeup();
    }
}

void EventLoop::runInLoop(Functor cb) {
    if (isInLoopThread()) {
        cb();
    } else {
        queueInLoop(std::move(cb));
    }
}

void EventLoop::runAt(Timestamp time, TimerCallback cb) {
    return timerQueue_->addTimer(std::move(cb), time);
}

void EventLoop::runAfter(double delay, TimerCallback cb) {
    Timestamp time(addTime(Timestamp::now(), delay));
    return runAt(time, std::move(cb));
}

void EventLoop::queueInLoop(Functor cb) {
    {
        MutexLockGuard guard(mutex_);
        pendingFunctors_.push_back(cb);
    }
    if (!isInLoopThread() || callingPendingFunctors_) {
        wakeup();
    }
}

void EventLoop::wakeup() {
    uint64_t one = 1;
    ssize_t n = ::write(wakeupFd_, &one, sizeof one);
    if (n != sizeof one) {
        std::cerr << "EventLoop::wakeup() writes " << n << " bytes instead of 8" << std::endl;
    }
}

void EventLoop::handleRead() {
    uint64_t one = 1;
    ssize_t n = ::read(wakeupFd_, &one, sizeof one);
    if (n != sizeof one) {
        std::cerr << "EventLoop::handleRead() reads " << n << " bytes instead of 8" << std::endl;
    }
}

void EventLoop::doPendingFunctors() {
    std::vector<Functor> functors;
    callingPendingFunctors_ = true;

    {
        MutexLockGuard lock(mutex_);
        functors.swap(pendingFunctors_);
    }
    for (const Functor &functor : functors) {
        functor();
    }
    callingPendingFunctors_ = false;
}


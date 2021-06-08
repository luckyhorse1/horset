//
// Created by xiaoma on 6/8/21.
//

#include "Channel.h"
#include "EventLoop.h"
#include <poll.h>

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;

Channel::Channel(EventLoop *loop, int fd) : loop_(loop), fd_(fd), events_(0), revents_(0), index_(-1) {

}

void Channel::handleEvent() {
    if (revents_ & (POLLIN | POLLPRI | POLLHUP)) {
        if (readCallback_) {
            readCallback_();
        }
    }
}

void Channel::update() {
    loop_->updateChannel(this);
}
//
// Created by xiaoma on 6/8/21.
//

#include "Poller.h"
#include "poller/PollPoller.h"

Poller * Poller::newDefaultPoller() {
    return new PollPoller();
}
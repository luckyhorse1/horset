//
// Created by xiaoma on 6/8/21.
//

#include "PollPoller.h"
#include <iostream>
#include <assert.h>

void PollPoller::poll(ChannelList *avtiveChannels) {
    int numEvents = ::poll(&*pollFdList.begin(), pollFdList.size(), 10000);
    if (numEvents > 0) {
        std::cout << numEvents << " events happened" << std::endl;
        fillActiveChannels(numEvents, avtiveChannels);
    } else if (numEvents == 0) {
        std::cout << "nothing happened" << std::endl;
    } else {
        std::cerr << "::poll failed" << std::endl;
    }
}

void PollPoller::fillActiveChannels(int numEvents, ChannelList *activeChannels) {
    for (auto it = pollFdList.begin(); it != pollFdList.end() && numEvents > 0; it++) {
        if (it->revents > 0) {
            numEvents--;
            auto ch = channelMap.find(it->fd);
            assert(ch != channelMap.end());
            Channel *channel = ch->second;
            assert(channel->fd() == it->fd);
            channel->setRevents(it->revents);
            activeChannels->push_back(channel);
        }
    }
}

void PollPoller::updateChannel(Channel *channel) {
    if (channel->index() < 0) {
        assert(channelMap.find(channel->fd()) == channelMap.end());
        struct pollfd pfd;
        pfd.fd = channel->fd();
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;
        pollFdList.push_back(pfd);
        int idx = static_cast<int>(pollFdList.size()) - 1;
        channel->setIndex(idx);
        channelMap[pfd.fd] = channel;
    } else {
        assert(channelMap.find(channel->fd()) == channelMap.end());
        assert(channelMap[channel->fd()] == channel);
        int idx = channel->index();
        assert(0 <= idx && idx < static_cast<int>(pollFdList.size()));
        struct pollfd &pfd = pollFdList[idx];
        assert(pfd.fd == channel->fd() || pfd.fd == -channel->fd() - 1);
        pfd.fd = channel->fd();
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;
        if (channel->isNoneEvent()) {
            // ignore this pollfd
            pfd.fd = -channel->fd() - 1;
        }
    }
}


#pragma once

#include "../Poller.h"
#include <vector>
#include <poll.h>

struct pollfd;

class PollPoller : public Poller {
public:
    void poll(ChannelList *avtiveChannels) override;

    void updateChannel(Channel *channel) override;


private:
    void fillActiveChannels(int numEvents, ChannelList *activeChannels);

    typedef std::vector<struct pollfd> PollFdList;
    PollFdList pollFdList;
};

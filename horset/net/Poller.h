#pragma once
#include <vector>
#include <map>
#include "Channel.h"

typedef std::vector<Channel*> ChannelList;
class Poller {
public:
    static Poller* newDefaultPoller();
    virtual void poll(ChannelList* avtiveChannels) = 0;
    virtual void updateChannel(Channel* channel) = 0;

protected:
    typedef std::map<int, Channel*> ChannelMap;
    ChannelMap channelMap;
};

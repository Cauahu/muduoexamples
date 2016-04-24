#ifndef __POLLER_H__
#define __POLLER_H__
#include <map>
#include <vector>
#include <muduo/base/Timestamp.h>
#include <boost/noncopyable.hpp>
#include "eventloop.h"

struct pollfd;
class  Channel;

class Poller : boost::noncopyable{
    public:
        typedef std::vector<Channel*> ChannelList;

        Poller(EventLoop*);
        ~Poller();

        void assertInLoopThread(){
            ownerLoop_->assertInLoopThread();
        }
        
        muduo::Timestamp poll(int, ChannelList*);
        void updateChannel(Channel*);
		void removeChannel(Channel*);
        void fillActiveChannels(int, ChannelList*);

    private:
        typedef std::vector<struct pollfd> PollFdList; 
        typedef std::map<int, Channel*>    ChannelMap;

        EventLoop* ownerLoop_;
        PollFdList pollfds_;
        ChannelMap channels_;
};

#endif  /*__POLLER_H__*/

#include "poller.h"
#include "channel.h"
#include <poll.h>

Poller::Poller(EventLoop* loop):ownerLoop_(loop)
{
}

Poller::~Poller(){
}

muduo::Timestamp Poller::poll(int timeoutMs, ChannelList* activeChannels){
	int numEvents = ::poll(&*pollfds_.begin(), pollfds_.size(), timeoutMs);
	if(numEvents > 0){
		LOG_TRACE << numEvents << "events happened";
		fillActiveChannels(numEvents, activeChannels);
	}
	else if(numEvents == 0){
		LOG_TRACE << "nothing happend";
	}
	else
		LOG_SYSERR << "Poller::poll()";
	muduo::Timestamp now = muduo::Timestamp::now();
	return now;
}

void Poller::fillActiveChannels(int numEvents, ChannelList* activeChannels){
	for(PollFdList::const_iterator it = pollfds_.begin();it != pollfds_.end() && numEvents > 0;++it){
		if(it->revents > 0){
			--numEvents;
			ChannelMap::const_iterator ch = channels_.find(it->fd);
			assert(ch != channels_.end());
			Channel* channel = ch->second;
			assert(channel->fd() == ch->first);
			channel->set_revents(it->revents);
			activeChannels->push_back(channel);
		}
	}
}


void Poller::updateChannel(Channel* channel){
	assertInLoopThread();
	LOG_TRACE << "fd = " << channel->fd() << " events = " << channel->events();

	if(channel->index() < 0){
		assert(channels_.find(channel->fd()) == channels_.end());
		struct pollfd pfd;
		pfd.fd = channel->fd();
		pfd.events = static_cast<short>(channel->events());
		pfd.revents = 0;
		pollfds_.push_back(pfd);

		int idx = static_cast<short> (pollfds_.size()-1);
		channel->setIndex(idx);
		channels_[pfd.fd] = channel;
	}
	else{
		assert(channels_.find(channel->fd()) != channels_.end());
		assert(channels_[channel->fd()] == channel);

		int idx = channel->index();
		assert(idx >= 0 && idx < static_cast<int>(pollfds_.size()));
		struct pollfd& pfd = pollfds_[idx];
		pfd.events = static_cast<short>(channel->events());
		pfd.revents = 0;
		if(channel->isNoneEvent())
			pfd.fd = -1;
	}
}

















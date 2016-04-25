#include "eventloop.h"
#include "channel.h"
#include "poller.h"
#include <assert.h>

__thread EventLoop* t_loopInThisThread = 0;
const int kPollTimeMs = 10000;
void EventLoop::loop(){
    assert(!looping_);
    assertInLoopThread();//make sure in I/O thread
    looping_ = true;
    quit_ = false;

    while(!quit_){
        activeChannels_.clear();
        poller_->poll(kPollTimeMs, &activeChannels_);
        for(ChannelList::iterator it = activeChannels_.begin();
                it != activeChannels_.end(); ++it)
            (*it)->handleEvent();
    }

    LOG_TRACE << "EventLoop " << this << " stop looping";
    looping_ = false;
}

EventLoop::EventLoop()
    :looping_(false),
     quit_(false),
     threadId_(muduo::CurrentThread::tid()),
     poller_(new Poller(this)){
         if(t_loopInThisThread) 
             LOG_FATAL << "Another EventLoop " << t_loopInThisThread 
                       << " exits in this thread" << threadId_;
         else
             t_loopInThisThread = this;
}

EventLoop::~EventLoop(){
    assert(!looping_);
    t_loopInThisThread = 0;
}


void EventLoop::updateChannel(Channel* channel){
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    poller_->updateChannel(channel);
}

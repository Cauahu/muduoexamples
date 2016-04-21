#include "eventloop.h"
#include "channel.h"
#include "poller.h"
#include "timer_queue.h"
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
     poller_(new Poller(this)),
     timerQueue_(new TimerQueue(this)){
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
    assert(channel->ownerloop() == this);
    assertInLoopThread();
    poller_->updateChannel(channel);
}

TimerId EventLoop::runAt(muduo::Timestamp time, const TimerCallback& cb){
    return timerQueue_->addTimer(cb, time, 0.0);
}

TimerId EventLoop::runAfter(double delay, const TimerCallback& cb){
    muduo::Timestamp time(addTime(muduo::Timestamp::now(), delay));
    return timerQueue_->addTimer(cb, time, 0.0);
}

TimerId EventLoop::runEvery(double interval, const TimerCallback& cb){
    muduo::Timestamp time(addTime(muduo::Timestamp::now(), interval));
    return timerQueue_->addTimer(cb, time, interval);
}

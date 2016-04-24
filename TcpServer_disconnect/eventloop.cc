#include "eventloop.h"
#include "channel.h"
#include "poller.h"
#include "timer_queue.h"
#include <assert.h>
#include <sys/eventfd.h>
#include <boost/bind.hpp>

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

        doPendingFunctors();
    }

    LOG_TRACE << "EventLoop " << this << " stop looping";
    looping_ = false;
}

int createEventfd(){
    int ret = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if(ret == -1){
        LOG_SYSERR << "failed create eventfd";
        abort();
    }
    return ret;
}

EventLoop::EventLoop()
    :looping_(false),
     quit_(false),
     threadId_(muduo::CurrentThread::tid()),
     poller_(new Poller(this)),
     timerQueue_(new TimerQueue(this)),
     wakeupId_(createEventfd()),
     wakeupChannel_(new Channel(this, wakeupId_)),
     callingPendingFunctors_(false),
     pendingFunctors_(){

         if(t_loopInThisThread) 
             LOG_FATAL << "Another EventLoop " << t_loopInThisThread 
                       << " exits in this thread" << threadId_;
         else
             t_loopInThisThread = this;

         wakeupChannel_->setReadCallback(
                 boost::bind(&EventLoop::handleRead, this));
         wakeupChannel_->enableReading();

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

void EventLoop::handleRead(){
    uint64_t  data;
    size_t  n = ::read(wakeupId_, &data, sizeof data);
    if(n != sizeof data)
        LOG_SYSERR << "EventLoop::handleRead() read " << n << "bytes instead of 8.";
}

void EventLoop::doPendingFunctors(){
    std::vector<Functor> temp;
    callingPendingFunctors_ = true;
    {
        muduo::MutexLockGuard lock(mutex_);
        temp.swap(pendingFunctors_);
    }
    for(size_t i = 0; i < temp.size(); ++i)
        temp[i]();
    callingPendingFunctors_ = false;
}


void EventLoop::runInLoop(const Functor& cb){
    if(isInLoopThread())
       cb();
    else
        queueInLoop(cb);
}

void EventLoop::queueInLoop(const Functor& cb){
    {
        muduo::MutexLockGuard lock(mutex_);
        pendingFunctors_.push_back(cb);
    }
    if(!isInLoopThread() || callingPendingFunctors_)
        wakeup();
}

void EventLoop::wakeup(){
    uint64_t data = 1;
    size_t n = ::write(wakeupId_, &data, sizeof data);
    if(n != sizeof data)
        LOG_SYSERR << "EventLoop::wakeup() write " << n 
                   << "bytes instead of 8.";
}

void EventLoop::removeChannel(Channel* channel){
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    poller_->removeChannel(channel);
}

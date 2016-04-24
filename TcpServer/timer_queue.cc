#include "timer_queue.h"
#include "eventloop.h"
#include "timer.h"
#include <sys/timerfd.h>
#include <boost/bind.hpp>
#include <stdint.h>

int createTimerfd(){
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if(timerfd < 0)
        LOG_SYSERR << "Failed create timerfd";
    return timerfd;
}

struct timespec howMuchTimeFromNow(muduo::Timestamp expiration){
    uint64_t microseconds = expiration.microSecondsSinceEpoch() -
                            muduo::Timestamp::now().microSecondsSinceEpoch();
    if(microseconds < 100)
        microseconds = 100;
    struct timespec ts;
    ts.tv_sec = static_cast<time_t>(
                microseconds / muduo::Timestamp::kMicroSecondsPerSecond);
    ts.tv_nsec = static_cast<long>((
                microseconds % muduo::Timestamp::kMicroSecondsPerSecond)* 1000);
    return ts;
}

void resetTimerfd(int timerfd, muduo::Timestamp when){
   struct itimerspec newvalue;
   bzero(&newvalue, sizeof newvalue);
   newvalue.it_value = howMuchTimeFromNow(when);
   int result = ::timerfd_settime(timerfd, 0, &newvalue, NULL);
   if(result)
       LOG_SYSERR << "timerfd_settime";
}

void readTimerfd(int timerfd, muduo::Timestamp now){
    uint64_t howmany;
    ssize_t n  = ::read(timerfd, &howmany, sizeof howmany);
    LOG_TRACE << "TimerQueue::handleRead() " << howmany 
              << " at " << now.toString();
    if( n  != sizeof howmany)
        LOG_ERROR << "TimerQueue::handleRead() read " << n 
                  << " bytes instead of 8.";
}


TimerQueue::TimerQueue(EventLoop* loop)
    :loop_(loop),
     timerfd_(createTimerfd()),
     timerfdChannel_(loop, timerfd_),
     timers_(){
        timerfdChannel_.setReadCallback(
                boost::bind(&TimerQueue::handleRead, this));
        timerfdChannel_.enableReading();
}
TimerQueue::~TimerQueue(){
    ::close(timerfd_);
    for(TimerList::iterator it = timers_.begin();
            it != timers_.end(); ++it)
        delete (it->second);
}

TimerId TimerQueue::addTimer(const TimerCallback& cb,
                             muduo::Timestamp when,
                             double interval){
    Timer* timer = new Timer(cb, when, interval);
    loop_->runInLoop(boost::bind(&TimerQueue::addTimerInLoop, this, timer));
    return TimerId(timer);
}

void TimerQueue::addTimerInLoop(Timer* timer){
    loop_->assertInLoopThread();
    bool earliestChanged = insert(timer);//first timer
    if(earliestChanged)
        resetTimerfd(timerfd_, timer->expiration());
}

bool TimerQueue::insert(Timer* timer){
    bool earliestChanged = false;
    muduo::Timestamp when = timer->expiration();
    TimerList::iterator it = timers_.begin();
    if(when < it->first || it == timers_.end())
        earliestChanged = true;
    std::pair<TimerList::iterator, bool>  result = 
        timers_.insert(std::make_pair(when, timer));
    assert(result.second);
    return earliestChanged;
}


void TimerQueue::handleRead(){
    loop_->assertInLoopThread();
    muduo::Timestamp now(muduo::Timestamp::now());
    readTimerfd(timerfd_, now);

    std::vector<Entry> expired = getExpired(now);
    for(std::vector<Entry>::const_iterator it = expired.begin();
            it != expired.end(); ++it)
        it->second->run();
    reset(expired, now);
}

std::vector<TimerQueue::Entry> TimerQueue::getExpired(muduo::Timestamp now){
    std::vector<TimerQueue::Entry> expired;
    Entry newEntry = std::make_pair(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
    TimerList::iterator it = timers_.lower_bound(newEntry);
    assert(it == timers_.end() || now < it->first);
    std::copy(timers_.begin(), it, back_inserter(expired));
    timers_.erase(timers_.begin(), it);
    return expired;
}

void TimerQueue::reset(const std::vector<Entry>& expired, muduo::Timestamp now){
    muduo::Timestamp nextExpired;
    for(std::vector<Entry>::const_iterator it = expired.begin();
            it != expired.end(); ++it){
        if(it->second->repeat()){
            it->second->restart(now);
            insert(it->second);
        }
        else
            delete it->second;
    }
    if(!timers_.empty())
        nextExpired = timers_.begin()->first;
    if(nextExpired.valid())
        resetTimerfd(timerfd_, nextExpired);
}


#ifndef __TIMER_QUEUE_H__
#define __TIMER_QUEUE_H__
#include <boost/noncopyable.hpp>
#include <muduo/base/Timestamp.h>
#include <vector>
#include <set>
#include "channel.h"
#include "callback.h"

class EventLoop;
class Timer;
class TimerId;

class TimerQueue : boost::noncopyable{
    public:
        TimerQueue(EventLoop*);
        ~TimerQueue();
        TimerId addTimer(const TimerCallback& cb,
                         muduo::Timestamp when,
                         double interval);
        void addTimerInLoop(Timer* timer);

    private:
        typedef std::pair<muduo::Timestamp, Timer*> Entry;
        typedef std::set<Entry>         TimerList;

        void handleRead();
        std::vector<Entry> getExpired(muduo::Timestamp now);
        void reset(const std::vector<Entry>& expired, muduo::Timestamp now);
        bool insert(Timer* timer);


        EventLoop* loop_;
        const int  timerfd_;
        Channel    timerfdChannel_;
        TimerList  timers_;
};

#endif  /*__TIMER_QUEUE_H__*/

#ifndef __EVENTLOOP_H__
#define __EVENTLOOP_H__
#include <muduo/base/Logging.h>
#include <muduo/base/CurrentThread.h>
#include <muduo/base/Mutex.h>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <pthread.h>
#include <vector>
#include "callback.h"
#include "timer.h"
#include <signal.h>

class Poller;
class Channel;
class TimerQueue;
class IgnoreSigPipe{
    public:
        IgnoreSigPipe(){
            ::signal(SIGPIPE, SIG_IGN);
        }
};

class EventLoop: boost::noncopyable{
    public:
        typedef std::vector<Channel*>    ChannelList;
        typedef boost::function<void ()> Functor;

        EventLoop();
        ~EventLoop();

        void assertInLoopThread(){
           if(!isInLoopThread()) 
               abortNotInLoopThread();
        }

        bool isInLoopThread()const{
            return threadId_ == muduo::CurrentThread::tid();
        }

        void abortNotInLoopThread(){
            LOG_FATAL << "EventLoop::abortNotInLoopThread - EventLoop " << this
                      << ", current thread id = " << muduo::CurrentThread::tid();
        }
        void updateChannel(Channel*);
        void removeChannel(Channel*);
        
        void quit(){
            quit_ = true;
            if(!isInLoopThread())
                wakeup();
        }

        void loop();

        TimerId runAt(muduo::Timestamp time, const TimerCallback&);
        TimerId runAfter(double delay,    const TimerCallback&);
        TimerId runEvery(double interval, const TimerCallback&);

        void runInLoop(const Functor&);
        void queueInLoop(const Functor&);
        void wakeup();
       
        
    private:
        void handleRead();
        void doPendingFunctors();

        bool        looping_;
        bool        quit_;
        const pid_t threadId_;
        ChannelList activeChannels_;
        boost::scoped_ptr<Poller>     poller_;
        boost::scoped_ptr<TimerQueue> timerQueue_;

        const    int                wakeupId_;
        boost::scoped_ptr<Channel>  wakeupChannel_;
        bool                        callingPendingFunctors_;
        muduo::MutexLock            mutex_;
        std::vector<Functor>        pendingFunctors_;
        IgnoreSigPipe initObj;
};


#endif  /*__EVENTLOOP_H__*/

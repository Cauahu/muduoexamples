#ifndef __EVENTLOOP_H__
#define __EVENTLOOP_H__
#include <muduo/base/Logging.h>
#include <muduo/base/CurrentThread.h>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <pthread.h>
#include <vector>
class Poller;
class Channel;

class EventLoop : boost::noncopyable{
    public:
        typedef std::vector<Channel*> ChannelList;
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
        
        void loop();
        void quit(){
            quit_ = true;
        }
    private:
        bool        looping_;
        bool        quit_;
        const pid_t threadId_;
        ChannelList activeChannels_;
        boost::scoped_ptr<Poller> poller_;
};


#endif  /*__EVENTLOOP_H__*/

#ifndef _EVENTLOOP_H_
#define _EVENTLOOP_H_
#include <muduo/base/Logging.h>
#include <muduo/base/CurrentThread.h>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <pthread.h>
#include <vector>
#include "callback.h"
#include "timer.h"

class Poller;
class Channel;
class TimerQueue;


class EventLoop:boost::noncopyable
{
	public :
		typedef std::vector<Channel*> ChannelList;
		EventLoop();
		~EventLoop();

		void assertInLoopThread()
		{
			if(!isInLoopThread())
			  abortNotInLoopThread();
		}

		bool isInLoopThread() const
		{
			return threadId_ == muduo::CurrentThread::tid();
		}

		void abortNotInLoopThread()
		{
			LOG_FATAL << "EventLoop::abortNotInLoopThread - EventLoop " << this << ", current thread id = " << muduo::CurrentThread::tid();
		}

		void updateChannel(Channel*);

		void loop();

		void quit()
		{
			quit_ = true;
		}

		TimerId runAt(muduo::Timestamp time, const TimerCallback&);
		TimerId runAfter(double delay, const TimerCallback&);
		TimerId runEvery(double interval, const TimerCallback&);

	private:
		bool looping_;
		bool quit_;
		const pid_t threadId_;
		ChannelList activeChannels_;
		boost::scoped_ptr<Poller> poller_;
		boost::scoped_ptr<TimerQueue> timerQueue_;

};

#endif

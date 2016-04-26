#ifndef __EVENTLOOP_THREAD_H__
#define __EVENTLOOP_THREAD_H__
#include <muduo/base/Mutex.h>
#include <muduo/base/Condition.h>
#include <muduo/base/Thread.h>
#include <boost/noncopyable.hpp>

class EventLoop;

class EventLoopThread : boost::noncopyable{
	public :
		EventLoopThread();
		~EventLoopThread();
		EventLoop* startLoop();

	private :
		void threadFunc();

		EventLoop* loop_;
		mudduo::Thread thread_;
		muduo::MutexLock mutex_;
		muduo::Condition cond_;
};
#endif

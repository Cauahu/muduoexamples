#ifndef _EVENTLOOP_H_
#define _EVENTLOOP_H_
#include <muduo/base/CurrentThread.h>
#include <muduo/base/Logging.h>
#include <boost/noncopyable.hpp>
#include <pthread.h>
#include <poll.h>
#include <assert.h>


class EventLoop;
__thread EventLoop* t_loopInThisThread = 0;

class EventLoop : boost::noncopyable{
	public :
		EventLoop():looping_(false),
		threadId_(muduo::CurrentThread::tid()){
			if(t_loopInThisThread)
			  LOG_FATAL << "Another EventLoop" << t_loopInThisThread
				  << "exits in this thread" << threadId_;
			else
			  t_loopInThisThread = this;		
		}
		~EventLoop(){
			assert(!looping_);
			t_loopInThisThread = 0;		
		}

		void loop(){			//启动
			assert(!looping_);
			assertInLoopThread();	//makesure in IO thread
			looping_ = true;

			::poll(NULL, 0, 5*1000);//等待5秒退出

			LOG_TRACE << "EventLoop " << this << "stop looping";
			looping_ = false;
		}

		void assertInLoopThread(){//检查one loop per thread 条件
			if(!isInLoopThread())    //线程id不是当前线程id
			  abortNotInLoopThread();//停止运行
		}

		bool isInLoopThread() const
		{
			return threadId_ == muduo::CurrentThread::tid();
		}

		void abortNotInLoopThread()
		{
			LOG_FATAL << "EventLoop::abortNotInLoopThread - EventLoop " << this << ", current thread id = " << muduo::CurrentThread::tid();
		}
	private:
		bool looping_;//是否在运行
		const pid_t threadId_;//loop所在线程id
};

#endif

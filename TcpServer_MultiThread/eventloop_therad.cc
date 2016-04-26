#include "eventloop_thread.h"
#include <boost/bind.hpp>
#include "eventloop.h"

EventLoopThread::EventLoopThread()
	:loop_(NULL),
	thread_(boost::bind(&EventLoopThread::threadFunc, this)),
	mutex_(),
	cond_(mutex_){
}

EventLoopThread::~EventLoopThread(){
	loop_->quit();
	thread_.join();
}

EventLoop* EventLoopThread::startLoop(){
	thread_.start();
	{
		muduo::MutexLockGuard lock(mutex_);
		while(loop_ == NULL)
			cond_.wait();
	}
	return loop_;
}

void EventLoopThread::threadFunc(){
	EventLoop loop;
	{
		muduo::MutexLockGuard lock(mutex_);
		loop_ = &loop;
		cond_.notify();
	}
	loop.loop;
}



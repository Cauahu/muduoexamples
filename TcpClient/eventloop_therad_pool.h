#ifndef __ENENTLOOP_THREAD_POOL_H__
#define __ENENTLOOP_THREAD_POOL_H__
#include <boost/ptr_container/ptr_vector.hpp>
#include <vector>
#include "eventloop_thread.h"

class EventLoop;

class EventLoopThreadPool{
	public :
	
	private :
		EventLoop* baseloop_;
		bool started_;
		int numThreads_;
		int next_;//round-robin 轮询方式
		boost::ptr_vector<EventLoopThread> threads_;
		std::vector<EventLoop*> loops_;
};
#endif

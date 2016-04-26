#include "eventloop_thread_pool.h"
#include "eventloop.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop)
	:baseLoop_(baseLoop),
	started_(false),
	numThreads_(0),
	next_(0){
}

EventLoopThreadPool::~EventLoopThreadPool(){
}

void EventLoopThreadPool::start(){
	assert(!started_);
	baseLoop_->assertInLoopThread();
	started_ = true;
	for(int i=0;i<numThreads_;++i){
		EventLoopThreadPool* thread = new EventLoopThread;
		threads_.push_back(thread);
		loops_.push_back(thread->startLoop());
	}
}

EventLoop* EventLoopThreadPool::getNextLoop(){
	baseLoop_->assertInLoopThread();
	EventLoop* loop = baseLoop_;
	if(!loops_.empty()){
		loop = loops_[next_++];
		if(static_cast<size_t>(next_) >= loops_.size())
			next_ = 0;
	}
	return loop;
}



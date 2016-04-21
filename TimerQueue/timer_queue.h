#ifndef	__TIMER_QUEUE_H__
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
	public :
		TimerQueue(EventLoop*);
		~TimerQueue();
		TimerId addTimer(const TimerCallback& cb,muduo::Timestamp when,double interval);

	private:
		typedef std::pair<muduo::Timestamp, Timer*> Entry;//记录单元
		typedef std::set<Entry> TimerList;//队列的数据结构

		void handleRead();
		std::vector<Entry> getExpired(muduo::Timestamp now);//获得到时定时器
		void reset(const std::vector<Entry>& expired, muduo::Timestamp now);//重置定时器
		bool insert(Timer* timer);//插入定时器

		EventLoop* loop_;
		const int timerfd_;
		Channel timerfdChannel_;
		TimerList timers_;
};
#endif

#ifndef __TIMER_H__
#define __TIMER_H__
#include <muduo/base/Timestamp.h>
#include "callback.h"
#include <boost/noncopyable.hpp>

class Timer : boost::noncopyable{

	public :
		Timer(const TimerCallback& cb, muduo::Timestamp when, double interval)
			:callback_(cb),
			expiration_(when),
			interval_(interval),
			repeat_(interval_ > 0.0){
		}

		void run() const {
			callback_();
		}

		muduo::Timestamp expiration() const{
			return expiration_;
		}

		bool repeat() const {
			return repeat_;
		}

		void restart(muduo::Timestamp now){
			if(repeat_){
				expiration_ = addTime(now, interval_);
			}
			else
				expiration_ = muduo::Timestamp::invalid();
		}

	private:
		const TimerCallback callback_;
		muduo::Timestamp expiration_;//Unix时间戳，终止时间点。
		const double interval_;//循环时间间隔
		const bool repeat_;
};

class TimerId{
	public :
		explicit TimerId(Timer* timer)
			:value_(timer){
		}
	private:
		Timer* value_;
};

#endif

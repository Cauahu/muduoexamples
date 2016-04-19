#ifndef __CHANNEL_H__
#define __CHANNEL_H__
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

class EventLoop;
class Channel : boost::noncopyable
{
	public:
		typedef boost::function<void ()> EventCallback;
		
		Channel(EventLoop* loop, int fdArg)
			:loop_(loop),
			fd_(fdArg),
			events_(0),
			revents_(0),
			index_(-1){
				
			}
			
		void setReadCallback(const EventCallback& cb){
			readCallback_ = cb;
		}
		void setWriteCallback(const EventCallback& cb){
			writeCallback_ = cb;
		}
		void setErrorCallback(const EventCallback& cb){
			errorCallback_ = cb;
		}
		int fd() const{
			return fd_;
		}
		int events() const{
			return events_;
		}
		void set_revents(int revt){
			revents_ = revt;
		}
		bool isNoneEvent() const{
			return events_ == kNoneEvent;
		}
		void enableReading() {
			events_ |= kReadEvent;
			update();
		}
		int index() const{
			return index_;
		}
		void setIndex(int idx){
			index_ = idx;
		}
		EventLoop* ownerloop(){
			return loop_;
		}
		
		void handleEvent();
		
	private:
		
		void update();
		
		EventLoop* loop_;
		const int fd_;
		int events_;
		int revents_;
		int index_;//在poller的结构pollfds中的下标

		EventCallback readCallback_;
		EventCallback writeCallback_;
		EventCallback errorCallback_;
		
		static const int kNoneEvent;
		static const int kReadEvent;
		static const int kWriteEvent;		
};
#endif

#ifndef __ACCEPTOR_H__
#define __ACCEPTOR_H__
#include "channel.h"
#include "socket.h"
#include "inet_address.h"
#include <boost/function.hpp>
#include <boost/bind.hpp>

class EventLoop;
class Acceptor{
	public :
		typedef boost::function<void (int fd, const InetAddress&)> NewConnectionCallback;
		Acceptor(EventLoop*, const InetAddress&);
		void listen();
		void setNewConnectionCallback(const NewConnectionCallback& cb){
			newConnectionCallback_ = cb;
		}
		bool listenning() const{
			return listenning_;
		}
	private :
		void handleRead();

		EventLoop* loop_;
		Socket acceptSocket_;
		Channel acceptChannel_;
		NewConnectionCallback newConnectionCallback_;
		bool listenning_;
};
#endif

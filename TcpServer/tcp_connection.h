#ifndef __TCP_CONNETION_H__
#define __TCP_CONNETION_H__
#include "callback.h"
#include "inet_address.h"
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

class EventLoop;
class Socket;
class Channel;

class TcpConnection : boost::noncopyable, public boost::enable_shared_from_this<TcpConnection>{
	public :
		TcpConnection(EventLoop*,
					const std::string&,
					int socketfd,
					const InetAddress&,
					const InetAddress&);
		void connectEstablished();
		void connectDestroyed();
		void setConnectionCallback(const ConnectionCallback& cb){
			connectionCallback_ = cb;
		}

		void setMessageCallback(const MessageCallback& cb){
			messageCallback_ = cb;
		}

		const std::string& name() const{
			return name_;
		}

		const InetAddress& peerAddress() const{
			return peerAddr_;
		}
		
		bool connected() const {
			return state_ == kConnected;
		}
	

	private :
		enum StateE{kConnecting, kConnected};
		void setState(StateE s){
			state_ = s;
		}
		
		void handleRead();
		void handleClose();
		void handleError();
		
		EventLoop* loop_;
		std::string name_;
		StateE state_;
		boost::scoped_ptr<Socket> socket_;
		boost::scoped_ptr<Channel> channel_;
		InetAddress localAddr_;
		InetAddress peerAddr_;
		ConnectionCallback connectionCallback_;
		CloseCallback closeCallback_;
		MessageCallback messageCallback_;


};
#endif

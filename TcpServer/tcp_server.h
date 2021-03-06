#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include "inet_address.h"
#include "callback.h"
#include <string>
#include <map>

class EventLoop;
class Acceptor;

class TcpServer : boost::noncopyable{
	public :
		TcpServer(EventLoop*, const InetAddress&);
		TcpServer(EventLoop*, const InetAddress&, const std::string&);
		~TcpServer();
		void start();
		void setConnectionCallback(const ConnectionCallback& cb){
			connectionCallback_ = cb;
		}

		void setMessageCallback(const MessageCallback& cb){
			messageCallback_ = cb;
		}

		void removeConnection(const TcpConnectionPtr& conn);

	private :
		void newConnection(int connfd, const InetAddress&);
		typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;

		EventLoop* loop_;
		const std::string name_;
		bool started_;
		int nextConnId_;
		boost::scoped_ptr<Acceptor> acceptor_;
		ConnectionCallback connectionCallback_;
		MessageCallback messageCallback_;
		ConnectionMap connections_;

};

#endif

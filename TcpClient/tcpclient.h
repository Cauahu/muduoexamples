#ifndef MUDUO_NET_TCPCLIENT_H
#define MUDUO_NET_TCPCLIENT_H

#include <boost/noncopyable.hpp>
#include <muduo/base/Mutex.h>
#include "TcpConnetion.h"

class Connector;
typedef boost :: shared_ptr<Connector> ConnectorPtr;

class TcpClient:boost::noncopyable{
	public :
		TcpClient(EventLoop* loop, const InetAddress& serverAddr, const string& nameArg);
		~TcpClient();

		void connect();
		void disconnect();
		void stop();

		TcpConnectionPtr connection() const{
			MutexLockGuard lock(mutex_);
			return connection_;
		}

		EventLoop* getLoop() const {return loop_;}
		bool retry() const;
		void enableRetry() { retry_ = true; }

		const string& name() const { return name_;}

		void setConnectionCallback(const ConnectionCallback& cb){
			connectionCallback_ = cb;
		}

		void setMessageCallback(const MessageCallback& cb){
			messageCallback_ = cb;	
		}

		void setWriteCompleteCallback(cosnt WriteConpleteCallback& cb){
			writeCompleteCallback_ = cb;
		}

	private :
		void newConnection(int sockfd);
		void removeConnection(const TcpConnectionPtr& conn);

		EventLoop* loop_;
		ConnectorPtr connector_;
		const string name_;
		ConnectorCallback connectionCallback_;
		MessageCallback messageCallback_;
		WriteCompleteCallback writeCompleteCallback_;
		bool retry_;
		bool connect_;
		int nextConnId_;
		mutable MutexLock mutex_;
		TcpConnectionPtr connection_;
};

#endif

#include "TcpClient.h"

#include <muduo/base/Logging.h>
#include <muduo/net/Connector.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/SocketsOps.h>

#include <boost/bind.hpp>
#include <stdio.h>

void removeConnection(EventLoop* loop, const TcpConnectionPtr& conn){
	loop->queueInLoop(boost::bind(&TcpConection::connectDestroyed, conn));
}

TcpClient::TcpClient(EventLoop* loop, const InetAddress& serverAddr, const string& nameArg)
	:loop_(loop),
	connector_(new Connector(loop, serverAddr)),
	name_(nameArg),
	retry_(false),
	connect_(true),
	nextConnId_(1){
		coonector_->setNewConnectionCallback(boost::bind(&TcpClient::newConnection, this, _1));
		LOG_INFO << "TcpClient::TcpClient[" << name_ << "] - connector " << get_pointer(connector_);
}

TcpClient::~TcpClient(){
	LOG_INFO << "TcpClient::~TcpClient[" << name_ << "] - connector " << get_pointer(connector_);
	TcpConnectionPtr conn;
	bool unique = false;
	{
		MutexLockGuard lock(mutex_);
		unique = connection_.unique();
		conn = connection_;
	}
	if(conn)
	{
		assert(loop_ == conn->getLoop());
		
		CloseCallback cb = boost::bind(removeConnection, _1);
		loop_->runInLoop(boost::bind(&TcpConection::setCloseCallback, conn, cb));
		if(unique)
		{
			conn->forceClose();
		}
	}
	else
	{
		conector_->stop();
		//loop_->runAfter(1,boost::bind(&T))
	}
}


void TcpClient::connect(){
	LOG_INFO << "TcpClient::connect [" << name_ << "] - connecting to " << connector_->serverAddress().toIpPort();
	connect_ = true;
	connector_->start();
}

void TcpClient::disconnect(){
	connect_ = false;
	{
		MutexLockGuard lock(mutex_);
		if(connection_)
		{
			connection_->shutdown();
		}
	}
}

void TcpClient::stop(){
	connect_ = false;
	connector_->stop();
}

void TcpClient::newConnection(int sockfd){
	loop_->assertInLoopThread();
	InetAddress peerAddr(sockets::getPeerAddr(sockfd));
	char buf[32];
	snprintf(buf, sizeof buf, ":%s#%d", peerAddr.toIpPort().c_str(),nextConnId_);
	++nextConnId_;
	string connName = name_ + buf;
	InetAddress localAddr(sockets::getLocalAddr(sockfd));
	TcpConnectionPtr conn(new TcpConection(loop_, connName, sockfd, localAddr, peerAddr));
	conn->setConnectionCallback(connectionCallback_);
	conn->setMessageCallback(messageCallback_);
	conn->setWriteCompleteCallback(writeCompleteCallback_);
	conn->setCloseCallback(boost::bind(&TcpClient::removeConnection, this, _1));
	{
		MutexLockGuard lock(mutex_);
		connection_ = conn;
	}
	conn->connectEstablished();
}


void TcpClient::removeConnection(const TcpConnectionPtr& conn){
	loop_->assertInLoopThread();
	assert(loop_ == conn->getLoop());
	{
		MutexLockGuard lock(mutex_);
		assert(connection_ == conn);
		connection_.reset();
	}
	
	loop_->queueInLoop(boost::bind(&TcpConection::connectDestroyed, conn));
	if(retry_ && connect_){
		LOG_INFO << "TcpClient::connect[" << name_ << "] - Reconnecting to "<< connector_->serverAddress().toIpPort();
		connector_->restart();
	}
}





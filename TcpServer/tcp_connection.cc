#include "tcp_connection.h"
#include "socket.h"
#include "channel.h"
#include "eventloop.h"
#include <boost/bind.hpp>

TcpConnection::TcpConnection(EventLoop* loop,
							const std::string& nameArg,
							int socketfd,
							const InetAddress& localAddr,
							const InetAddress& peerAddr)
	:loop_(loop),
	name_(nameArg),
	state_(kConnecting),
	socket_(new Socket(socketfd)),
	channel_(new Channel(loop, socketfd)),
	localAddr_(localAddr),
	peerAddr_(peerAddr){
		channel_->setReadCallback(boost::bind(&TcpConnection::handleRead, this));	
		channel_->setCloseCallback(boost::bind(&TcpConnection::handleClose, this));
		channel_->setErrorCallback(boost::bind(&TcpConnection::handleError, this));
}

void TcpConnection::handleRead(){
	char buf[65536];
	size_t n = ::read(channel_->fd(), buf, sizeof buf);
	if(n>0)
		messageCallback_(shared_from_this(), buf, n);
	else if(n == 0 )
		handleClose();
	else
		handleError();
}

void TcpConnection::handleClose(){
	assert(state_ == kDisconnect);
	channel_->disableAll();
	closeCallback_(shared_from_this());
}

void TcpConnection::handleError(){
	int err = sockets::getSocketError(channel_->fd());
	LOG_ERROR << " " << name_ << "" << err << "" << muduo::strerror_tl(err);
}

void TcpConnection::connectEstablished(){
	loop_->assertInLoopThread();
	assert(state_ == kConnecting);
	setState(kConnected);
	channel_->enableReading();
	connectionCallback_(shared_from_this());
}

void TcpConnection::connectDestroyed(){
	loop_->assertInLoopThread();
	assert(state_ == kConnected);
	setState(kDisconnect);
	channel_->disableAll();
	closeCallback_(shared_from_this());
	loop_->removeChannel(boost::get_pointer(channel_));
}



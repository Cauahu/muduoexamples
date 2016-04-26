#include "connector.h"
#include "eventloop.h"
#include "sockets_ops.h"
#include <muduo/base/Loggin.h>
#include <boost/bind.hpp>

const int Connector::kMaxRetryDelayMs;

Connector::Connector(EventLoop* loop, const InetAddress& serverAddr)
	:loop_(loop),
	serverAddr_(serverAddr),
	start_(false),
	state_(kDisconnect),
	retryDelayMs_(kInitRetryDelayMs){
	}

Connector::~Connector(){
}

void Connector::start(){
	start_ = true;
	loop_->runInLoop(boost::bind(&Connector::startInLoop, this));
}

void Connector::startInLoop(){
	loop_->assertInLoopThread();
	assert(state_ == kDisconnect );
	if(start_)
		doConnect();
	else
		LOG_ERROR << "Connector::startInLoop";
}

void Connector::retry(int sockfd){
	sockets::close(sockfd);
	loop_->runAfter(retryDelayMs_, boost::bind(&Connector::startInLoop, this));
	retryDelayMs_ = std::min(retryDelayMs_*2, kMaxRetryDelayMs);
}

void Connector::doConnect(){
	state_ = kConnecting;
	int sockfd = sockets::createNonblockOrDie();
	int ret = sockets::connect(sockfd, serverAddr_.getSockAddrInet());
	int saveErrno = (ret == 0) ? 0 : errno;
	switch(savedErrno){
		case 0:
		case EISCONN:
		case EINTR:
		case EINPROGRESS:
			connecting(sockfd);
			break;

		case EAGAIN:
		case EADDRINUSE:
		case EADDRNOTAVAIL:
		case ECONNREFUSED:
		case ENETUNREACH:
			retry(sockfd);
			break;

		case EACCES:
		case EAFNOSUPPORT:
		case EALREADY:
		case EBADF:
		case EFAULT:
		case ENOTSOCK:
		case EPERM:
			sockets::close(sockfd);
			break;

		default:
			sockets::close(sockfd);
			break;
	}
}

void Connector::connecting(int sockfd){
	setState(kConnecting);
	assert(!channel_);
	channel_.reset(new Channel(loop_, sockfd));
	channel_.setWriteCallback(boost::bind(&Connector::handlWrite, this));
	channel_.setErrorCallback(boost::bind(&Connector::handleError, this));
	channel_.enableWriting();
}

void Connector::setState(State newState){
	state_ = newState;
}

void Connector::handleWrite(){
	int sockfd = removeAndResetChannel();
	int err = sockets::getSocketError(sockfd);
	if(err)
		retry(sockfd);
	else if(sockets::isSelfConnect(sockfd))
		retry(sockfd);
	else{
		setState(kConnected);
		if(start_)
			newConnectionCallback_(sockfd);
		else
			sockets::close(sockfd);
	}
}

int Connector::removeAndResetChannel(){
	channel_->disableAll();
	loop_->removeChannel(boost::get_pointer(channel_));
	int sockfd = channel_->fd();
	loop_->queueInLoop(boost::bind(&Connector::resetChannel, this));
	return sockfd;
}

void Connector::handleError(){
	int sockfd = removeAndResetChannel();
	int err = sockets::getSocketError(sockfd);
	LOG_TRACE << "SO_ERROR" << err;
	retry(sockfd);
}

void Connector::resetChannel(){
	channel_.reset();
}

void Connector::restart(){
	loop_->assertInLoopThread();
	setState(kDisconnect);
	start_ = true;
	startInLoop();
}

void Connector::stop(){
	start_ = false;
}













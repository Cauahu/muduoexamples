#include "acceptor.h"
#include "sockets_ops.h"
#include "eventloop.h"
#include <utility>

Acceptor::Acceptor(EventLoop* loop, const InetAddress& listenAddr)
	:loop_(loop),
	acceptSocket_(sockets::createNonblockOrDie()),
	acceptChannel_(loop, acceptSocket_.fd()),
	listenning_(false){
		acceptSocket_.setReuseAddr(true);
		acceptSocket_.bindAddress(listenAddr);
		acceptChannel_.setReadCallback(boost::bind(&Acceptor::handleRead, this));
}

void Acceptor::listen(){
	loop_->assertInLoopThread();
	listenning_ = true;
	acceptSocket_.listen();
	acceptChannel_.enableReading();
}

void Acceptor::handleRead(){
	loop_->assertInLoopThread();
	InetAddress peerAddr(0);
	int connfd = acceptSocket_.accept(&peerAddr);
	if(connfd >= 0){
		if(newConnectionCallback_)
			newConnectionCallback_(connfd, peerAddr);
		else
			sockets::close(connfd);
	}
}


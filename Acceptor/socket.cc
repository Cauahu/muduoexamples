#include "socket.h"
#include "sockets_ops.h"
#include <strings.h>

Socket::~Socket(){
	sockets::close(sockfd_);
}

void Socket::bindAddress(const InetAddress& inetAddr){
	sockets::bindOrDie(sockfd_, inetAddr.getSockAddrInet());
}

void Socket::listen(){
	sockets::listenOrDie(sockfd_);
}

int Socket::accept(InetAddress* peerAddr){
	struct sockaddr_in addr;
	bzero(&addr, sizeof (struct sockaddr_in));
	int connfd = sockets::accept(sockfd_, &addr);//accept函数返回一个新的套接字，服务端使用这个新的套接字与客户端通信，而原来的sockfd继续用于监听其他客户端的连接请求。
	if(connfd >= 0)
		peerAddr->setSockAddrInet(addr);
	return connfd;
}

void Socket::setReuseAddr(bool on){
	int optval = on ? 0 : 1;
	::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);
}

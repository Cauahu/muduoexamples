#include "socket.h"
#include "sockets_ops.h"
#include <strings.h>    //bzero

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
    int connfd = sockets::accept(sockfd_, &addr);
    if(connfd >= 0)
        peerAddr->setSockAddrInet(addr);
    return connfd;
}

void Socket::setReuseAddr(bool on){
    int optval = on ? 0 : 1;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR,
                 &optval, sizeof optval);
}

void Socket::shutdownWrite(){
    sockets::shutdownWrite(sockfd_);
}


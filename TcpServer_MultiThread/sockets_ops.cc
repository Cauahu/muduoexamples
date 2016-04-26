#include "sockets_ops.h"
#include <muduo/base/Logging.h>
#include <sys/socket.h> 
#include <stdio.h>  //sprintf
#include <unistd.h> //close
#include <errno.h>


typedef struct sockaddr SA;
SA* sockets::sockaddr_cast(struct sockaddr_in* addr){
    return static_cast<SA*>(muduo::implicit_cast<void*>(addr));
}

const SA* sockets::sockaddr_cast(const struct sockaddr_in* addr){
    return static_cast<const SA*>(muduo::implicit_cast<const void*>(addr));
}
int  sockets::createNonblockOrDie(){
    int sockfd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sockfd < 0){
        LOG_SYSFATAL << "sockets::socket()";
        abort();
    }
    return sockfd;
}

void sockets::bindOrDie(int sockfd, const struct sockaddr_in& localAddr){
    int ret = ::bind(sockfd, sockaddr_cast(&localAddr), sizeof (struct sockaddr_in));
    if(ret < 0)
        LOG_SYSFATAL << "sockets::bindOrDie()";
}

void sockets::listenOrDie(int sockfd){
    int ret = ::listen(sockfd, SOMAXCONN);
    if(ret < 0)
        LOG_SYSFATAL << "sockets::listen()";
}

int  sockets::accept(int sockfd, struct sockaddr_in* peerAddr){
    socklen_t len = sizeof(struct sockaddr_in);
    int connfd = ::accept4(sockfd, sockaddr_cast(peerAddr), 
                           &len, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if(connfd < 0)
        LOG_SYSFATAL << "sockets::accept()";
    return connfd;
}

void sockets::close(int sockfd){
    if(::close(sockfd) < 0)
        LOG_SYSFATAL << "sockets::close()";
}

void sockets::shutdownWrite(int sockfd){
    ::shutdown(sockfd, SHUT_WR);
}
void sockets::fromHostPort(const char* ip, uint16_t port,
                  struct sockaddr_in* addr){
    addr->sin_family = AF_INET;
    addr->sin_port = hostToNetwork16(port);
    if(::inet_pton(AF_INET, ip, &(addr->sin_addr)) <= 0)
        LOG_SYSFATAL << "sockets::fromHostPort()";
}

void sockets::toHostPort(char* buf, size_t size, 
                const struct sockaddr_in& addr){
    char host[INET_ADDRSTRLEN] = "INVALID";
    ::inet_ntop(AF_INET, &addr.sin_addr, host, sizeof host);
    uint16_t port = networkToHost16(addr.sin_port);
    snprintf(buf, size, "%s:%u", host, port);
}

struct sockaddr_in sockets::getLocalAddr(int sockfd){
    struct sockaddr_in localAddr;
    bzero(&localAddr, sizeof (struct sockaddr));
    socklen_t len = sizeof (struct sockaddr);
    if(::getsockname(sockfd, sockaddr_cast(&localAddr), &len) < 0)
        LOG_SYSERR << "sockets::getLocalAddr";
    return localAddr;
}


int sockets::getSocketError(int fd){
    int optval;
    socklen_t optlen = sizeof optval;
    if(::getsockopt(fd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0)
        return errno;
    else
        return optval;

}



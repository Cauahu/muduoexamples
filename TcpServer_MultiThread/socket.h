#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <boost/noncopyable.hpp>
#include "inet_address.h"

class Socket : boost::noncopyable{
    public:
        explicit Socket(int sockfd)
            :sockfd_(sockfd){
        }
        ~Socket();
        int fd() const{
            return sockfd_;
        }
        void bindAddress(const InetAddress& inetAddr);
        void listen();
        int  accept(InetAddress* peerAddr);
        void setReuseAddr(bool on);
        void shutdownWrite();
        void setTcpNoDelay(bool);
        void setKeepAlive(bool);

    private:
        const int sockfd_;

};

#endif  /*__SOCKET_H__*/

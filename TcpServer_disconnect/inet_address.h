#ifndef __INET_ADDRESS_H__
#define __INET_ADDRESS_H__ 

#include <netinet/in.h> //sockaddr_in
#include <stdint.h>     //uint16_t
#include <string>

class InetAddress{
    public:
        explicit InetAddress(uint16_t port);
        InetAddress(const std::string& ip, uint16_t port);
        InetAddress(const struct sockaddr_in&);
        void setSockAddrInet(const struct sockaddr_in&);
        const struct sockaddr_in& getSockAddrInet() const;
        std::string toHostPort() const;

    private:
        struct sockaddr_in addr_;
};

#endif  /*__INET_ADDRESS_H__*/

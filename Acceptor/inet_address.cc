/*
 *封装IPV4地址格式
 *
 *
 */
#include "inet_address.h"
#include "sockets_ops.h"
#include <strings.h>	//bzero

static const in_addr_t kInaddrAny = INADDR_ANY;//32位IP地址的二进制形式。

InetAddress::InetAddress(uint16_t port){
	bzero(&addr_, sizeof addr_);
	addr_.sin_family = AF_INET;
	addr_.sin_port = sockets::hostToNetwork16(port);//将短值由主机字节序转换为网络字节序
	addr_.sin_addr.s_addr = sockets::hostToNetwork32(kInaddrAny);
}

InetAddress::InetAddress(const std::string& ip, uint16_t port){
	bzero(&addr_, sizeof addr_);
	sockets::fromHostPort(ip.c_str(), port, &addr_);
}

InetAddress::InetAddress(const struct sockaddr_in& addr):addr_(addr){
}

void InetAddress::setSockAddrInet(const struct sockaddr_in& addr){
	addr_ = addr;
}

const struct sockaddr_in& InetAddress::getSockAddrInet() const {
	return addr_;
}

std::string InetAddress::toHostPort() const{
	char buf[32];
	sockets::toHostPort(buf, sizeof buf, addr_);
	return buf;

}

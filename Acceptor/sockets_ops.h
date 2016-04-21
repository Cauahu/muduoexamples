#ifndef __SOCKETS_OPS_H__
#define __SOCKETS_OPS_H__

#include <sys/types.h>
#include <arpa/inet.h>

namespace sockets{
	int createNonblockOrDie();
	void bindOrDie(int sockfd, const struct sockaddr_in& localAddr);
	void listenOrDie(int sockfd);
	int accept(int sockfd, struct sockaddr_in* peerAddr);
	void close(int sockfd);
	struct sockaddr* sockaddr_cast(struct sockaddr_in*);
	const struct sockaddr* sockaddr_cast(const struct sockaddr_in* addr);

	inline uint32_t hostToNetwork32(uint32_t host32){
		return htonl(host32);
	}

	inline uint16_t hostToNetwork16(uint16_t host16){
		return htons(host16);
	}
	
	inline uint16_t networkToHost16(uint16_t host16){
		return ntohs(host16);
	}
	void fromHostPort(const char* ip, uint16_t port,struct sockaddr_in* addr);
	void toHostPort(char* buf, size_t size, const struct sockaddr_in& addr);

}
#endif

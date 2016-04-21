#include "timer_queue.h"
#include "eventloop.h"
#include "acceptor.h"
#include "sockets_ops.h"
#include <stdio.h>

void newConnection(int sockfd, const InetAddress& peerAddr){
	printf("newConnection(): accepted a new connection from %s\n",peerAddr.toHostPort().c_str());
	::write(sockfd, "How are you?\n", 13);
	sockets::close(sockfd);
}

int main(int argc, const char *argv[]){
	printf("main(): pid: %d, tid: %d\n", getpid(), muduo::CurrentThread::tid());
	EventLoop loop;
	InetAddress listenAddr(9999);
	Acceptor acceptor(&loop, listenAddr);

	acceptor.setNewConnectionCallback(newConnection);
	acceptor.listen();
	loop.loop();

	return 0;
}

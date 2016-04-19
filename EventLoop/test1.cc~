#include <muduo/base/Thread.h>
#include <stdio.h>
#include "eventloop.h"

void threadFunc()
{
	printf("threadFunc(): pid = %d ,tid = %d\n", getpid(), muduo::CurrentThread::tid());
	EventLoop loop;
	loop.loop();
}

int main(int argc, const char *argv[])
{
	
	printf("threadFunc(): pid = %d ,tid = %d\n", getpid(), muduo::CurrentThread::tid());
	EventLoop loop;

	muduo::Thread thread(threadFunc);
	thread.start();
	loop.loop();

	pthread_exit(NULL);
	return 0;
}

#include <muduo/base/Thread.h>
#include "eventloop.h"
#include <stdio.h>

EventLoop* g_loop;
void threadFunc()
{
	g_loop->loop();
}

int main(int argc, const char * argv[])
{
	EventLoop loop;
	g_loop = &loop;
	printf("pid = %d \n", getpid());
	muduo::Thread thread(threadFunc);
	thread.start();
	thread.join();

	return 0;
}

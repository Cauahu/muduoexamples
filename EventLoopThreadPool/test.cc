#include <muduo/net/EventLoop.h>
#include <muduo/net/EventLoopThread.h>
#include <muduo/net/EventLoopThreadPool.h>

#include <stdio.h>

using namespace muduo;
using namespace muduo::net;

void runInThread()
{
//	std::cout << "runInThread(): name = " << CurrentThread::name() << 
//		", tid=" << CurrentThread::tid() << std::endl;
	printf("runInThread(): name=%s, tid=%d\n",CurrentThread::name(), CurrentThread::tid());
}

int main(int argc, char* argv[])
{
	runInThread();

	EventLoop loop;
	EventLoopThreadPool loopthreadpool(&loop, "sub reactor");
	loopthreadpool.setThreadNum(5);
	loopthreadpool.start();
	for(int i=0;i<10;i++)
	{
		EventLoop* loopfrompool = loopthreadpool.getNextLoop();
		loopfrompool->runInLoop(runInThread);
	}
	sleep(3);
}


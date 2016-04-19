#include<sys/timerfd.h>
#include<stdio.h>
#include "eventloop.h"
#include "channel.h"

EventLoop* g_loop;
void timeout(){
	printf("timeout\n");
	g_loop->quit();
}


int main(int argc, const char *argv[]){
	EventLoop loop;
	g_loop = &loop;

	int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC );
	Channel channel(&loop, timerfd);
	channel.setReadCallback(timeout);
	channel.enableReading();//将channel加入到poller中，通过其中的update方法。

	struct itimerspec howlong;
	bzero(&howlong, sizeof howlong);
	howlong.it_value.tv_sec = 5;
	::timerfd_settime(timerfd, 0, &howlong, NULL);

	loop.loop();
	::close(timerfd);
	return 0;
}



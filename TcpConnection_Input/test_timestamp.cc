#include "eventloop.h"
#include "channel.h"
#include <muduo/base/Timestamp.h>
#include <sys/timerfd.h>
#include <stdio.h>
#include <strings.h>

EventLoop* g_loop;
void timeout(muduo::Timestamp receiveTime){
    printf("timeout     : %s\n", 
            receiveTime.toFormattedString().c_str());
    g_loop->quit();
}

int main(int argc, const char *argv[])
{
    printf("time started: %s\n", 
            muduo::Timestamp::now().toFormattedString().c_str());
    EventLoop loop;
    g_loop = &loop;

    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    assert(timerfd > 0);
    Channel channel(&loop, timerfd);
    channel.setReadCallback(timeout);
    channel.enableReading();

    struct itimerspec new_value;
    bzero(&new_value, sizeof (struct itimerspec));
    new_value.it_value.tv_sec = 5;
    int ret = ::timerfd_settime(timerfd, 0, &new_value, NULL);
    assert(ret == 0);

    loop.loop();
    ::close(timerfd);
    return 0;
}

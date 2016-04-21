#include "timer_queue.h"
#include "eventloop.h"
#include <muduo/base/Timestamp.h>
#include <boost/bind.hpp>
#include <stdio.h>
#include <muduo/base/CurrentThread.h>

EventLoop* g_loop;
int g_count = 0;

void print(const char* msg){
    printf("msg: %s %s\n", muduo::Timestamp::now().toString().c_str(),
            msg);
    if(++g_count == 6)
        g_loop->quit();
}

int main(int argc, const char *argv[])
{
    printf("pid: %d, tid: %d\n", getpid(), muduo::CurrentThread::tid());
    printf("now: %s\n", muduo::Timestamp::now().toString().c_str());

    EventLoop loop;
    g_loop = &loop;

    loop.runAfter(1, boost::bind(print, "once1"));
    loop.runAfter(1.5, boost::bind(print, "once1.5"));
    loop.runAfter(2, boost::bind(print, "once2"));
    loop.runEvery(2, boost::bind(print, "every2"));

    loop.loop();
    printf("main loop exit\n");
    sleep(1);
    return 0;
}

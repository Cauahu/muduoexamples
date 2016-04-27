#include "eventloop.h"
#include "connector.h"
#include "eventloop_thread_pool.h"
#include <stdio.h>
#include <vector>
#include <muduo/base/Thread.h>

using namespace muduo;
using namespace std;
InetAddress serverAddr("127.0.0.1", 9999);

void connectCallback(int sockfd){
    printf("%d is connected\n", sockfd);
}

void threadFunc(){
    EventLoop thisLoop;
    ConnectorPtr connector(new Connector(&thisLoop, serverAddr));
    connector->setNewConnectionCallback(connectCallback);
    connector->start();
    thisLoop.loop();
}

int main(int argc, const char *argv[])
{
    printf("main(): pid: %d, tid: %d\n", 
            getpid(), muduo::CurrentThread::tid());

    int numConnectors = 1;
    if(argc > 1){
       numConnectors = atoi(argv[1]);
    }

    vector<Thread*> threads;
    for(int i = 0; i < numConnectors; ++i)
        threads.push_back(new Thread(threadFunc));

    for(int i = 0; i < numConnectors; ++i){
        threads[i]->start();
    }
    for(int i = 0; i < numConnectors; ++i){
        threads[i]->join();
    }
    sleep(3);
    return 0;
}

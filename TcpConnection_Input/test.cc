#include "timer_queue.h"
#include "eventloop.h"
#include "tcp_server.h"
#include "tcp_connection.h"
#include "sockets_ops.h"
#include <stdio.h>

void onConnection(const TcpConnectionPtr& conn){
    if(conn->connected())
        printf("onConnection(): new connection [%s] from %s\n",
                conn->name().c_str(), conn->peerAddress().toHostPort().c_str());
    else
        printf("onConnection(): connection [%s] is down",
                conn->name().c_str());


}

void onMessage(const TcpConnectionPtr& conn,
               Buffer* buf,
               muduo::Timestamp receiveTime){
    printf("onMessage():  received %zd bytes from connection [%s] at %s\n", 
            buf->readableBytes(), 
            conn->name().c_str(),
            receiveTime.toFormattedString().c_str());
    printf("onMessage: [%s]\n", buf->retriveAllAsString().c_str());
}

int main(int argc, const char *argv[])
{
    printf("main(): pid: %d, tid: %d\n", 
            getpid(), muduo::CurrentThread::tid());
    
    EventLoop loop;
    InetAddress listenAddr(9999);

    TcpServer server(&loop, listenAddr);
    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);

    server.start();
    loop.loop();

    return 0;
}

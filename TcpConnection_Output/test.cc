#include "timer_queue.h"
#include "eventloop.h"
#include "tcp_server.h"
#include "tcp_connection.h"
#include "sockets_ops.h"
#include <stdio.h>
#include <algorithm>

std::string message1;
std::string message2;

void onConnection(const TcpConnectionPtr& conn){
    printf("main: onConnection callback\n");
    if(conn->connected()){
        printf("onConnection(): new connection [%s] from %s\n",
                conn->name().c_str(), conn->peerAddress().toHostPort().c_str());
        conn->send(message1);
        conn->send(message2);
//        conn->shutdown();
    }
    else{
        LOG_INFO << " [be informed that connection was closed]";
        printf("onConnection(): connection [%s] is down\n",
                conn->name().c_str());
    }
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
    int len1 = 100;
    int len2 = 200;
    message1.resize(len1);
    message2.resize(len2);
    std::fill(message1.begin(), message1.begin() + len1, 'A');
    std::fill(message2.begin(), message2.begin() + len2, 'B');
    
    EventLoop loop;
    InetAddress listenAddr(9999);

    TcpServer server(&loop, listenAddr);
    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);

    server.start();
    loop.loop();

    return 0;
}

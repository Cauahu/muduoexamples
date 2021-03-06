#include "tcp_server.h"
#include "eventloop.h"
#include "acceptor.h"
#include "callback.h"
#include "sockets_ops.h"
#include "tcp_connection.h"
#include <boost/get_pointer.hpp>
#include <stdio.h>

TcpServer::TcpServer(EventLoop* loop, const InetAddress& inetAddr)
    :loop_(loop),
     name_(inetAddr.toHostPort()),
     started_(false),
     nextConnId_(1),
     acceptor_(new Acceptor(loop, inetAddr)){
         acceptor_->setNewConnectionCallback(
                 boost::bind(&TcpServer::newConnection, this, _1, _2));
}

TcpServer::TcpServer(EventLoop* loop, const InetAddress& inetAddr, 
                     const std::string& name)
    :loop_(loop),
     name_(name),
     started_(false),
     nextConnId_(1),
     acceptor_(new Acceptor(loop, inetAddr)){
         acceptor_->setNewConnectionCallback(
                 boost::bind(&TcpServer::newConnection, this, _1, _2));
}

TcpServer::~TcpServer(){}

void TcpServer::start(){
    if(!started_)
        started_ = true;
    if(!acceptor_->listenning())
        loop_->runInLoop(boost::bind(&Acceptor::listen, boost::get_pointer(acceptor_)));
}

void TcpServer::newConnection(int connfd, const InetAddress& peerAddr){
    loop_->assertInLoopThread();
    char buf[32];
    snprintf(buf, sizeof buf, "#%d", nextConnId_);
    ++nextConnId_;
    std::string connName = name_ + buf;
    LOG_INFO << "TcpServer::newConnection [" << name_
             << "] - new connection ["       << connName
             << "] from " << peerAddr.toHostPort();
    const InetAddress& localAddr = sockets::getLocalAddr(connfd);
    
    TcpConnectionPtr conn(new TcpConnection(loop_, connName, connfd, localAddr, peerAddr));
    connections_[connName] =  conn;
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);
    conn->setCloseCallback(
            boost::bind(&TcpServer::removeConnection, this, _1));
    conn->connectEstablished();
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn){
    LOG_INFO << " [CloseCallback is TcpServer::removeConnection]";
    LOG_INFO << " [TcpConnectionPtr usecount:]" << conn.use_count();
    loop_->assertInLoopThread();
    LOG_INFO << "TcpServer::removeConnection [" << name_
             << "] - connection " << conn->name();
    size_t n = connections_.erase(conn->name());
    LOG_INFO << " [ After erase one in connections_, TcpConnectionPtr usecount:]" 
             << conn.use_count();
    assert(n == 1);
    loop_->runInLoop(
            boost::bind(&TcpConnection::connectDestroyed, conn));
}

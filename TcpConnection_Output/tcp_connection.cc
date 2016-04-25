#include "tcp_connection.h"
#include "socket.h"
#include "sockets_ops.h"
#include "channel.h"
#include "eventloop.h"
#include <boost/bind.hpp>

TcpConnection::TcpConnection(EventLoop* loop, 
                             const std::string& nameArg, 
                             int socketfd, 
                             const InetAddress& localAddr, 
                             const InetAddress& peerAddr)
    :loop_(loop),
     name_(nameArg),
     state_(kConnecting),
     socket_(new Socket(socketfd)),
     channel_(new Channel(loop, socketfd)),
     localAddr_(localAddr),
     peerAddr_(peerAddr){
        channel_->setReadCallback(
                boost::bind(&TcpConnection::handleRead, this, _1));
        channel_->setWriteCallback(
                boost::bind(&TcpConnection::handleWrite, this));
        channel_->setCloseCallback(
                boost::bind(&TcpConnection::handleClose, this));
        channel_->setErrorCallback(
                boost::bind(&TcpConnection::handleError, this));
}

TcpConnection::~TcpConnection(){
    LOG_INFO << " [~TcpConnection]";
}

void TcpConnection::handleRead(muduo::Timestamp receiveTime){
    int errorNo = 0;
    size_t n = inputBuffer_.readFd(channel_->fd(), &errno);
    if(n > 0)
        messageCallback_(shared_from_this(), &inputBuffer_, receiveTime);
    else if(n == 0){
        LOG_INFO << " [handleRead(): readFd  return 0]";
        handleClose();
    }
    else{
        errno = errorNo;
        LOG_ERROR << "TcpConnection::handleRead()";
        handleError();
    }
}

void TcpConnection::handleClose(){
    assert(state_ == kConnected || state_ == kDisconnecting);
    channel_->disableAll();
    LOG_INFO << " [disableAll  and closeCallback_]";
    closeCallback_(shared_from_this());
}

void TcpConnection::handleError(){
    int err = sockets::getSocketError(channel_->fd());
    LOG_ERROR << "TcpConnection:handleError ["   << name_
              << "] - SO_ERROR = " << err << " " << muduo::strerror_tl(err);
}

void TcpConnection::connectEstablished(){
    loop_->assertInLoopThread();
    assert(state_ == kConnecting);
    setState(kConnected);
    channel_->enableReading();
    connectionCallback_(shared_from_this());
}

void TcpConnection::connectDestroyed(){
    LOG_INFO << " [TcpConnection::connectDestroyed]";
    loop_->assertInLoopThread();
    assert(state_ == kConnected || state_ == kDisconnecting);
    setState(kDisconnected);
    channel_->disableAll();
    connectionCallback_(shared_from_this());
    loop_->removeChannel(boost::get_pointer(channel_));
    LOG_INFO << " [after Poller::removeChannel]";
}

void TcpConnection::send(const std::string& message){
    if(state_ == kConnected){
        if(loop_->isInLoopThread())
            sendInLoop(message);
        else
            loop_->runInLoop(
                    boost::bind(&TcpConnection::sendInLoop, this, message));
    }
}

void TcpConnection::sendInLoop(const std::string& message){
    loop_->assertInLoopThread();
    ssize_t nwrote = 0;
    if(!channel_->isWriting() && outputBuffer_.readableBytes() == 0){
        nwrote = ::write(channel_->fd(), message.data(), message.size());
        if(nwrote >= 0){
            if(muduo::implicit_cast<size_t>(nwrote) < message.size())
                LOG_TRACE << "I'm going to write more data.";
        }
        else{
            nwrote = 0;
            if(errno != EWOULDBLOCK)
                LOG_SYSERR << "TcpConnection::sendInLoop()";
        }
    }

    assert(nwrote >= 0);
    if(muduo::implicit_cast<size_t>(nwrote) < message.size()){
        outputBuffer_.append(message.data() + nwrote, message.size() - nwrote);
        if(!channel_->isWriting())
            channel_->enableWriting();
    }
}

void TcpConnection::shutdown(){
    if(state_ == kConnected){
        setState(kDisconnecting);
        loop_->runInLoop(
                         boost::bind(&TcpConnection::shutdownInLoop, this));
    }
}

void TcpConnection::shutdownInLoop(){
    loop_->assertInLoopThread();
    if(!channel_->isWriting())
        socket_->shutdownWrite();
    LOG_INFO << " [After shutdown]";
}

void TcpConnection::handleWrite(){
    loop_->assertInLoopThread();
    if(channel_->isWriting()){
       ssize_t n = ::write(channel_->fd(), 
                           outputBuffer_.peek(), 
                           outputBuffer_.readableBytes()) ;
       if(n > 0){
           outputBuffer_.retrive(n);
           if(0 == outputBuffer_.readableBytes()){
               channel_->disableWriting();
               if(state_ == kDisconnecting)
                   shutdownInLoop();
           }
           else
               LOG_TRACE << "more data to wirte";
       }
       else
           LOG_SYSERR << "TcpConnection::handleWrite()";
    }
    else 
        LOG_TRACE << "connection is down, no more write.";
}



#ifndef __TCP_CONNECTION_H__
#define __TCP_CONNECTION_H__
#include "callback.h"
#include "inet_address.h"
#include "buffer.h"
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

class EventLoop;
class Socket;
class Channel;

class TcpConnection : boost::noncopyable,
                      public boost::enable_shared_from_this<TcpConnection>{
    public:
        TcpConnection(EventLoop*, 
                      const std::string&, 
                      int socketfd, 
                      const InetAddress&, 
                      const InetAddress&);

        
        ~TcpConnection();
        void setConnectionCallback(const ConnectionCallback& cb){
            connectionCallback_ = cb;
        }

        void setMessageCallback(const MessageCallback& cb){
            messageCallback_ = cb;
        }
        void setCloseCallback(const CloseCallback& cb){
            closeCallback_ = cb;
        }
        void setWriteCompleteCallback(const WriteCompleteCallback& cb){
            writeCompleteCallback_ = cb; 
        }
        void setHighWaterMarkCallback(const HighWaterMarkCallback& cb,
                                      size_t highWaterMark){
            highWaterMarkCallback_ = cb;
            highWaterMark_ = highWaterMark;
        }

        const std::string& name()const{
            return name_;
        }

        const InetAddress& peerAddress() const{
            return peerAddr_;
        }

        bool connected()const{
            return state_ == kConnected;
        }

        void send(const std::string& message);
        void shutdown();
        void connectEstablished();
        void connectDestroyed();
        void setTcpNoDelay(bool);

    private:
        enum StateE{ kConnecting, kConnected, kDisconnecting, kDisconnected};

        void setState(StateE s){
            state_ = s;
        }

        void handleRead(muduo::Timestamp receiveTime);
        void handleWrite();
        void handleClose();
        void handleError();

        void sendInLoop(const std::string& message);
        void shutdownInLoop();

        EventLoop* loop_;
        std::string name_;
        StateE state_;
        boost::scoped_ptr<Socket>  socket_;
        boost::scoped_ptr<Channel> channel_;
        InetAddress localAddr_;
        InetAddress peerAddr_;
        ConnectionCallback connectionCallback_;
        CloseCallback      closeCallback_;
        MessageCallback messageCallback_;
        WriteCompleteCallback writeCompleteCallback_;
        HighWaterMarkCallback highWaterMarkCallback_;
        size_t highWaterMark_;  
        Buffer  inputBuffer_;
        Buffer  outputBuffer_;
};

#endif  /*__TCP_CONNECTION_H__*/

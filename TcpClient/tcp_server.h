#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include "inet_address.h"
#include "callback.h"
#include "eventloop_thread_pool.h"
#include <string>
#include <map> 

class EventLoop;
class Acceptor; 

class TcpServer : boost::noncopyable{
    public:
        TcpServer(EventLoop*, const InetAddress&);
        TcpServer(EventLoop*, const InetAddress&, const std::string&);
        ~TcpServer();
        void start();

        void setConnectionCallback(const ConnectionCallback& cb){
            connectionCallback_ = cb;
        }
        void setMessageCallback(const MessageCallback& cb){
            messageCallback_ = cb;
        }
        void setWriteCompleteCallback(const WriteCompleteCallback& cb){
            writeCompleteCallabck_ = cb;
        }
        void removeConnection(const TcpConnectionPtr& conn);
        void removeConnectionInThread(const TcpConnectionPtr& conn);

        void setThreadNum(int numThreads){
            pool_->setThreadNum(numThreads);
        }
    private:
        void newConnection(int connfd, const InetAddress&);

        typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;

        EventLoop* loop_;
        const std::string name_;
        bool started_;
        int nextConnId_;
        boost::scoped_ptr<Acceptor> acceptor_;
        ConnectionCallback connectionCallback_;
        MessageCallback    messageCallback_;
        WriteCompleteCallback writeCompleteCallabck_;
        ConnectionMap      connections_;
        boost::scoped_ptr<EventLoopThreadPool> pool_;
};

#endif  /*__TCP_SERVER_H__*/

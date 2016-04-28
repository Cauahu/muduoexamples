#include "echo.h"

#include <muduo/base/Logging.h>

#include <boost/bind.hpp>
#include <ctype.h>
#include <algorithm>
// using namespace muduo;
// using namespace muduo::net;

//rot13 encode
struct op_trans{
    int operator()(const int& c){
        if(isalpha(c)){
            int tmp = c + 13;
            if(isupper(c))
                return tmp > 'Z' ? tmp - 26 : tmp;
            else
                return tmp > 'z' ? tmp - 26 : tmp;
        }
        else 
            return c;
    }
};
EchoServer::EchoServer(muduo::net::EventLoop* loop,
                      const muduo::net::InetAddress& listenAddr)
  : server_(loop, listenAddr, "EchoServer")
{
  server_.setConnectionCallback(
      boost::bind(&EchoServer::onConnection, this, _1));
  server_.setMessageCallback(
      boost::bind(&EchoServer::onMessage, this, _1, _2, _3));
}

void EchoServer::start()
{
  server_.start();
}

void EchoServer::onConnection(const muduo::net::TcpConnectionPtr& conn)
{
  LOG_INFO << "EchoServer - " << conn->peerAddress().toIpPort() << " -> "
           << conn->localAddress().toIpPort() << " is "
           << (conn->connected() ? "UP" : "DOWN");
}

void EchoServer::onMessage(const muduo::net::TcpConnectionPtr& conn,
                           muduo::net::Buffer* buf,
                           muduo::Timestamp time)
{
  muduo::string msg(buf->retrieveAllAsString());
  LOG_INFO << conn->name() << " echo " << msg.size() << " bytes, "
           << "data received at " << time.toString();
  std::transform(msg.begin(), msg.end(), msg.begin(), op_trans());
  conn->send(msg);
}





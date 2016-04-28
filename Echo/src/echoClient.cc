#include "echoClient.h"
#include <muduo/net/Buffer.h>
#include <boost/bind.hpp>
#include <unistd.h>
#include <iostream>

EchoClient::EchoClient(EventLoop* loop,
		const InetAddress& serverAddr,
		const string& nameArg):
	client_(loop, serverAddr, nameArg),
	channel_(loop, )
{
	client_.setConnectionCallback(boost::bind(&EchoClient::onConnection, this, _1));
	client_.setMessageCallback(boost::bind(&EchoClient::onMessage, this, _1,_2,_3));
	channel_.enableReading();
	channel_.setReadCallback(boost::bind(&EchoClient::send, this));
}

void EchoClient::onConnection(const TcpConnectionPtr& conn)
{
	if(conn->connected())
		std::cout << "Connect to " << conn->peerAddress().toIpPort() << "successfully" << std::endl;
	else
	std::cout << "Connect to " << conn->peerAddress().toIpPort() << "failed" << std::endl;
}

void EchoClient::onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp time)
{
	std::cout << "Receie : " << buf->retrieveAllAsString() << std::endl;
}

void EchoClient::send(){
	string msg("this is a test program!");
	//std::cin >> msg;
	Buffer buf;
	buf.append(msg);
	client_.connection()->send(&buf);
}

void EchoClient::start()
{
	client_.connect();
}
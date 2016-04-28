#include <muduo/base/Logging.h>
#include <muduo/base/Mutex.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpClient.h>
#include <muduo/net/Buffer.h>

#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>

#include <iostream>
#include <stdio.h>

using namespace muduo;
using namespace muduo::net;

class EchoClient : boost::noncopyable
{
	public :
		EchoClient(EventLoop* loop, const InetAddress& serverAddr)
			:client_(loop, serverAddr, "EchoClient")
		{
			client_.setConnectionCallback(boost::bind(&EchoClient::onConnection, this, _1));
			client_.setMessageCallback(boost::bind(&EchoClient::onMessage, this, _1, _2, _3));
			client_.enableRetry();
		}
		void connect()
		{
			client_.connect();
		}

		void disconnect()
		{
			client_.disconnect();
		}
		
		void write(const StringPiece& message)
		{
			MutexLockGuard lock(mutex_);
			if(connection_)
			{
				std::cout << "connection is ok" << std::endl;
				connection_->send(message);
			}
		}

	private :
		void onConnection(const TcpConnectionPtr& conn)
		{
			LOG_INFO << conn->localAddress().toIpPort() << " -> "
				<< conn->peerAddress().toIpPort() << " is "
				<< (conn->connected()? "UP":"DOWN");
			MutexLockGuard lock(mutex_);
			if(conn->connected())
			{
				connection_ = conn;
			}
			else
			{
				connection_.reset();
			}
		}

		void onMessage(const TcpConnectionPtr& ,
				Buffer* buf,
				Timestamp)
		{
			muduo::string message(buf->retrieveAllAsString());
			LOG_INFO << message;
		}


		TcpClient client_;
		TcpConnectionPtr connection_;
		MutexLock mutex_;
};

int main()
{
	EventLoop loop;
	InetAddress serverAddr("127.0.0.1", 2007);

	EchoClient client(&loop, serverAddr);
	client.connect();
	std::string line;
	while(std::getline(std::cin,line))
	{
		std::cout << line << std::endl;
		client.write(line);
	}
	client.disconnect();

}


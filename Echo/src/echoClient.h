#include <muduo/net/TcpClient.h>
#include <muduo/net/Channel.h>
#include <muduo/net/TcpConnection.h>
#include <muduo/net/InetAddress.h>
#include <muduo/base/Timestamp.h>
#include <muduo/net/EventLoop.h>

using namespace muduo;
using namespace muduo::net;

class EchoClient{
	public :
		EchoClient(EventLoop* loop, const InetAddress& serverAddr, const string& nameArg);
		void start();
		void send();
	
	private :
		void onConnection(const TcpConnectionPtr& con);
		void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp time);
		TcpClient client_;
		Channel channel_;
};

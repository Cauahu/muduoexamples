#include "echoClient.h"

int main()
{
	EventLoop loop;
	InetAddress serverAddr("127.0.0.1", 2007 );
	EchoClient client(&loop, serverAddr, "echoclient");
	client.start();
	loop.loop();
	return 0;
}

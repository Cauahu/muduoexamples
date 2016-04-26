#ifndef __CONNECTOR_H__
#define __CONNECTOR_H__

#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include "inet_address.h"
#include "channel.h"

class EventLoop;
class Connector : boost::noncopyable{

	public :
		typedef boost::function<void (int sockfd)> NewConnectionCallback;

		Connector(EventLoop* loop, const InetAddress& serverAddr);
		~Connector();
		
		void setNewConnectionCallback(const NewConnectionCallback& cb){
			newConnectionCallback_ = cb;
		}

		void start();
		void restart();
		void stop();

	private :
		enum State{kDisconnect, kConnecting, kConnected};
		static const int kMaxRetryDelayMs = 30 * 1000;
		static const int kInitRetryDelayMs = 500;

		void startInLoop();
		void doConnect();
		void connecting(int);
		void handleWrite();
		void handleError();
		void setState(State);
		int removeAndResetChannel();
		void resetChannel();
		void retry(int);

		EventLoop* loop_;
		InetAddress serverAddr_;
		bool start_;
		State state_;
		int retryDelayMs_;
		boost::scoped_ptr<Channel> channel_;
		NewConnectionCallback newConnectionCallback_;
};
typedef boost::shared_ptr<Connector> ConnectorPtr;

#endif


#include <muduo/net/TcpServer.h>

#include <boost/circular_buffer.hpp>
#include <boost/unordered_set.hpp>

using namespace muduo;
using namespace muduo::net;
class EchoServer
{
	public :
		EchoServer(EventLoop* loop,
				const InetAddress& listenAddr,
				int idleSeconds);
		void start();

	private :
		void onConnection(const TcpConnectionPtr& conn);
		void onMessage(const TcpConnectionPtr& conn,
				Buffer* buf,
				Timestamp time);

		void onTimer();

		void dumpConnectionBuckets() const;

		typedef boost::weak_ptr<TcpConnection> WeakTcpConnectionPtr;

		struct Entry : public muduo::copyable
		{
			explicit Entry(const WeakTcpConnectionPtr& weakConn)
				:weakConn_(weakConn)
			{
			}

			~Entry()
			{
				TcpConnectionPtr conn = weakConn_.lock();
				if(conn)
				{
					conn->shutdown();
				}
			}
			WeakTcpConnectionPtr weakConn_;
		};

		typedef boost::shared_ptr<Entry> EntryPtr;
		typedef boost::weak_ptr<Entry> WeakEntryPtr;
		typedef boost::unordered_set<EntryPtr> Bucket;
		typedef boost::circular_buffer<Bucket> WeakConnectionList;

		TcpServer server_;
		WeakConnectionList connectionBuckets_;

};


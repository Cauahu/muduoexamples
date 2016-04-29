#include <muduo/net/EventLoop.h>

#include <iostream>
#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>

using namespace muduo;
using namespace muduo::net;



class Printer : boost::noncopyable
{
	public :
		Printer(EventLoop* loop)
			:loop_(loop),
			count_(0)
		{
			loop_->runEvery(1, boost::bind(&Printer::print, this));
		}

		~Printer()
		{
			std::cout << "Final count is " << count_ << std::endl;
		}

		void print()
		{
			if(count_ < 5)
			{
				std::cout << count_ << "\n";
				++count_;
			}
			else
			{
				loop_->quit();
			}
		}
	private :
		EventLoop* loop_;
		int count_;
};

int main()
{
	muduo::net::EventLoop loop;
	Printer printer(&loop);
	loop.loop();
}


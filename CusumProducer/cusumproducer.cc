#include <muduo/base/Mutex.h>
#include <muduo/base/Thread.h>
#include <muduo/base/Condition.h>
#include <boost/bind.hpp>

#include <iostream>

using namespace muduo;

struct msg{
	struct msg *next;
	int num;
};

struct msg *head;

MutexLock mutex_;
Condition con_(mutex_);

void consumer()
{
	struct msg *mp;
	for(;;)
	{
		{
			MutexLockGuard lock(mutex_);
			while(head == NULL)
				con_.wait();
			mp = head;
			head = mp->next;
		}

		std::cout << "Consume:" << mp->num << std::endl ;
		free(mp);
		sleep(2);
	}
}

void producer()
{
	struct msg *mp;
	for(;;)
	{
		mp = new struct msg;
		mp->num = rand()%1000+1;
		std::cout << "Produce: " << mp->num << std::endl ;
		{
			MutexLockGuard lock(mutex_);
			mp->next = head;
			head = mp;
		}
		con_.notify();
		sleep(2);

	}
}

int main(int argc, char * argv[])
{
	Thread threadone(producer, "Producer");
	Thread threadtwo(consumer, "Consumer");

	threadone.start();
	threadtwo.start();
	
	threadone.join();
	threadtwo.join();

	return 0;
}




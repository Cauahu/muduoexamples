#include <muduo/base/Mutex.h>
#include <boost/noncopyable.hpp>
#include <pthread.h>

namespace muduo
{
	class Condition:boost::noncopyable
	{
		public :
			explicit Condition():mutex_(mutex)
			{
				pthread_cond_init(&pcond_, NULL);
			}
			~Condition()
			{
				pthread_cond_destroy(&pcond_);
			}
			void wait()
			{
				MutexLock::UnassignGuard ug(mutex_);
				pthread_cond_wait(&pcond_, mutex.getPthreadMutex());
			}

			bool waitForSeconds(int seconds);

			void notify()
			{
				pthread_cond_signal(&pcond_);
			}

			void notifyAll()
			{
				pthread_cond_broadcast(&pcond_);
			}

		private :
			MutexLock& mutex_;
			pthread_cond_t pcond_;
	};
}

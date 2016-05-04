#include <muduo/base/CurrentThread.h>
#include <boost/noncopyable.hpp>
#include <assert.h>
#include <pthread.h>

namespace muduo
{
	class MutexLock : boost::noncopyable
	{
		public :
			MutexLock()
				:holder_(0)
			{
				pthread_mutex_init(&mutex_, NULL);
			}
			~MutexLock()
			{
				assert(holder == 0);
				pthread_mutex_destroy(&mutex_);
			}

			bool isLockedByThisThread() const
			{
				return holder_ == CurrentThread::tid();
			}

			void assertLocked() const
			{
				assert(isLockedByThisThread());
			}

			void lock()
			{
				pthread_mutex_lock(&mutex_);
				assignHolder();
			}

			void unlock()
			{
				unassignHolder();
				pthread_mutex_unlock(&mutex_);
			}

			pthread_mutex_t* getPthreadMutex()
			{
				return &mutex_;
			}


		private :
			friend class Condition;

			class UnassignGuard : boost::noncopyable
			{
				public :
					UnassignGuard(MutexLock& owner):owner_(owner)
					{
						owner_.unassingnHolder();
					}
					~UnassignGuard()
					{
						owner_.assignHolder();
					}
				private :
					MutexLock& owner_;
			}

			void unassignHolder()
			{
				holder_ = 0;
			}

			void assignHolder()
			{
				holder_ = CurrentThread::tid();
			}

			pthread_mutex_t mutex_;
			pid_t holder_;

	};

	class MutexLockGuard : boost::noncopyable
	{
		public :
			explicit MutexLockGuard(MutexLock& mutex)
				:mutex_(mutex)
			{
				mutex_.lock();
			}

			~MutexLockGuard
			{
				mutex_.unlock();
			}
		private :
			MutexLock& mutex_;
	};

}

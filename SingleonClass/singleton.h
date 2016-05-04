#include <boost/noncopyable.hpp>
#include <assert.h>
#include <stdlib.h>
#include <pthread.h>

namespace muduo
{
	template<typename T>
		class Singleton:boost::noncopyable
		{
			public :
				static T& instance()
				{
					pthread_once(&ponce_, &Singleton::init);
					assert(value_ != NULL);
					return *value_;
				}
			
			private :
				Singleton();
				~Singleton();

				static void init()
				{
					value_ = new T();
				}

				static pthread_once_t ponce_;
				static T* value_;
		};
	template<typename T>
		pthread_once_t Singleton<T>::ponce = PTHREAD_ONCE_INIT;
	template<typename> T
		T* Singleton<T>::value_ = NULL;
}

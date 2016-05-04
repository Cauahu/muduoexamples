#include <muduo/base/copyable.h>
#include <muduo/base/Types.h>
#include <boost/operators.hpp>

namespace muduo
{
	class Timestamp : public muduo::copyable
	{
		public :
			Timestamp():microSecondsSincEpoch_(0)
			{
			}

			explicit Timestamp(int64_t microSecondsSincEpochArg)
				:microSecondsSincEpoch_(microSecondsSincEpochArg)
			{
			}

			void swap(Timestamp& that)
			{
				std::swap(microSecondsSincEpoch_, that.microSecondsSincEpoch_);
			}

			string toString() const;
			string toFormattedString(bool showMicroseconds = true) const;

			bool valid() const {return microSecondsSincEpoch_ > 0}

			int64_t microSecondsSincEpoch() const {return microSecondsSincEpoch_;}
			time_t secondsSinceEpoch() const
			{
				return static_cast<time_t>(microSecondsSincEpoch_ / kMicroSecondsPerSecond);
			}

			static Timestamp now();
			static Timestamp invalid()
			{
				return Timestamp();
			}

			static Timestamp fromUnixTime(time_t t)
			{
				return fromUnixTime(t, 0);
			}

			static Timestamp fromUnixTime(time_t t, int microseconds)
			{
				return Timestamp(static_cast<int64_t>(t) * kMicroSecondsPerSecond + microseconds);
			}

			static const int kMicroSecondsPerSecond = 1000*1000;

		private :
			int64_t microSecondsSincEpoch_;
	
	};

	inline bool operator<(Timestamp lhs, Timestamp rhs)
	{
		return lhs.microSecondsSincEpoch() < rhs.microSecondsSincEpoch();
	}

	inline bool operator==(Timestamp lhs, Timestamp rhs)
	{
		return lhs.microSecondsSincEpoch() == rhs.microSecondsSincEpoch();
	}
	
	inline double timeDifference(Timestamp high, Timestamp low)
	{
		int64_t diff = high.microSecondsSincEpoch() - low.microSecondsSincEpoch();
		return static_cast<double>(diff) / Timestamp::kMicroSecondsPerSecond;
	}

	inline Timestamp addTime(Timestamp timestamp, double seconds)
	{
		int64_t delta = static_cast<int64_t>(seconds * Timestamp::kMicroSecondsPerSecond);
		return Timestamp(timestamp.microSecondsSincEpoch() + delta);
	}

}

#include <muduo/base/copyable.h>
#include <muduo/base/StringPiece.h>
#include <muduo/base/Types.h>

#include <muduo/net/Endian.h>

#include <algorithm>
#include <vector>

#include <assert.h>
#include <string.h>

class Buffer:public muduo::copyable
{
	public :
		static const size_t kCheapPrepend = 8;
		static const size_t kInitialSize = 1024;

		explicit Buffer(size_t initialSize = kInitialSize)
			:buffer_(kCheapPrepend+initialSize),
			readerIndex_(kCheapPrepend),
			writerIndex_(kCheapPrepend)
	{
		assert(readableBytes == 0);
		assert(writableBytes() == initialSize);
		assert();
	}
	private :
		char* begin()
		{
			return &*buffer_.begin();
		}

		void makeSpace(size_t len)
		{
			if(writableBytes() + prependableBytes() < len + kCheapPrepend)
				buffer_.resize(writerIndex_+len);
			else
			{
				assert(kCheapPrepend < readerIndex_);
				size_t readable = readableBytes();
				std::copy(begin()+readerIndex_,begin()+writerIndex_,begin()+kCheapPrepend);
				readerIndex_ = kCheapPrepend;
				writerIndex_ = readerIndex_ + readable;
				assert(readable == readableBytes());
			}
		}


		std::vector<char> buffer_;
		size_t readerIndex_;
		size_t writerIndex_;
}

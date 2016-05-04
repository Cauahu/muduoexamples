#include "FileUtil.h"

#include <muduo/base/Logging.h>

#include <boost/static_assert.hpp>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>

using namespace muduo::FileUtil;

AppendFile::AppendFile(StringArg filename)
	:fp_(::fopen(filename.c_str(), "ae")),
	writtenBytes_(0)
{
	assert(fp_);
	::setbuffer(fp_, buffer_, sizeof buffer_);
}

AppendFile::~AppendFile()
{
	::fclose(fp_);
}

void AppendFile::append(const char* logline, const size_t len)
{
	size_t n = write(logline, len);
	size_t remain = len - n;
	while(remain > 0)
	{
		size_t x = write(logline + n, remain);
		if(x == 0)
		{
			int err = ferror(fp_);
			if(err)
			{
				fprintf(stderr, "AppendFile::append() failed %s\n", strerror_tl(err));
			}
			break;
		}
		n += x;
		remain = len -n;
	}
	writtenBytes_ += len;
}

void AppendFile::flush()
{
	::flush(fp_);
}

size_t AppendFile :: write(const char* logline, size_t len)
{
	return ::fwrite_unlocked(logline, 1, len, fp_);
}

ReadSmallFile::ReadSmallFile(StringArg filename)
	: fd_(::open(filename.c_str(), O_RDONLY | O_CLOEXEC)),
	err(0)
{
	buf[0] = '\0';
	if(fd_ < 0)
	{
		err_ = errno;
	}
}

ReadSmallFile::~ReadSmallFile()
{
	if(fd_ >= 0)
	{
		::close(fd_);
	}
}


template<typename String>
int ReadSmallFile::readToString(int maxSize,
		String* content,
		int64_t* fileSize,
		int64_t* modifyTime,
		int64_t* createTime)
{
	assert(content != NULL);
	int err = err_;
	if(fd_ >= 0)
	{
		content->clear();
		if(fileSize)
		{
			struct stat statbuf;
			if(::fstat(fd_, &statbuf) == 0)
			{
				if(S_ISREG(statbuf.st_mode))
				{
					
				}
			}
		}
	}

}



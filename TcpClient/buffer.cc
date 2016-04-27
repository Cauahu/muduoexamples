#include "buffer.h"
#include <sys/uio.h>

size_t Buffer::readFd(int fd, int* errorNo){
    char extrabuf[65535];
    size_t leftLen = writableBytes();
    struct iovec iovecArr[2];
    iovecArr[0].iov_base = begin() + writerIndex_;
    iovecArr[0].iov_len  = leftLen; 
    iovecArr[1].iov_base = extrabuf; 
    iovecArr[1].iov_len  = sizeof extrabuf;
    ssize_t n = readv(fd, iovecArr, 2);
    if(n < 0)
        *errorNo = errno;
    else if(static_cast<size_t>(n) < leftLen)
        writerIndex_ += n;
    else{
        writerIndex_ = buffer_.size();
        append(extrabuf, n - leftLen);
    }
    return n;
}

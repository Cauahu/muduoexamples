#ifndef __BUFFER_H__
#define __BUFFER_H__

#include <stdlib.h>
#include <vector>
#include <assert.h>
#include <string.h>
#include <algorithm>
#include <sys/uio.h>
#include <errno.h>
#include <string>


class Buffer{
    public:
        static const size_t kCheapPrepend = 8;
        static const size_t kInitialSize  = 128;

        explicit Buffer(size_t initialSize = kInitialSize)
            :buffer_(kCheapPrepend + initialSize),
            readerIndex_(kCheapPrepend),
            writerIndex_(kCheapPrepend){
                assert(readableBytes() == 0);
                assert(writableBytes() == initialSize);
                assert(prependableBytes() == kCheapPrepend);
            }

        size_t readableBytes() const{
            return writerIndex_ - readerIndex_;
        }
        size_t writableBytes(){
            return buffer_.size() - writerIndex_;
        }
        size_t size(){
            return kCheapPrepend + kInitialSize;
        }
        size_t prependableBytes(){
            return readerIndex_;
        }
        void swap(Buffer& rhs){
            buffer_.swap(rhs.buffer_);
            std::swap(readerIndex_, rhs.readerIndex_);
            std::swap(writerIndex_, rhs.writerIndex_); 
        }
        const char* peek() const{
            return begin() + readerIndex_;
        }
        char* beginWrite() {
            return begin() + writerIndex_;
        }
        const char* beginWrite() const{
            return begin() + writerIndex_;
        }

        void retrive(size_t len){
            assert(len <= readableBytes());
            if(len <= readableBytes())
                readerIndex_ += len;
            else
                retriveAll();
        }

        void retriveAll(){
            readerIndex_ = kCheapPrepend;
            writerIndex_ = kCheapPrepend;
        }

        std::string retriveAllAsString(){
            std::string ret(peek(), readableBytes());
            retriveAll();
            return ret;
        }

        void append(const char* data, size_t len){
            ensureWritableBytes(len);
            std::copy(data, data + len, begin() + writerIndex_);
            hasWritten(len);
        }
        void read(char* data, size_t len){
            assert(readableBytes() >= len);
            const char* s = begin() + readerIndex_;
            std::copy(s, s + len, data);
            retrive(len);
        }

        void prepend(const void* data, size_t len){
            assert(len <= prependableBytes());
            const char* d = static_cast<const char*>(data);
            std::copy_backward(d, d + len, begin() + readerIndex_);
        }

        void hasWritten(size_t len){
            assert(len <= writableBytes());
            writerIndex_ += len;
        }

        void ensureWritableBytes(size_t len){
            while(writableBytes() < len)
                makeSpace(len);
            assert(writableBytes() >= len);
        }
        size_t readFd(int fd, int* errorNo);


    private:
        char* begin(){
            return &*buffer_.begin();
        }
        const char* begin() const{
            return &*buffer_.begin();
        }

        void makeSpace(size_t len){
            if(prependableBytes() + writableBytes() < len + kCheapPrepend)
                buffer_.resize(writerIndex_ + len);
            else{
                std::copy(begin() + readerIndex_, begin() + readableBytes(),
                        begin() + kCheapPrepend);
                readerIndex_ = kCheapPrepend;
                writerIndex_ = readerIndex_ + readableBytes();
            }
        }


        std::vector<char> buffer_;
        size_t readerIndex_;
        size_t writerIndex_;
};


#endif  /*__BUFFER_H__*/

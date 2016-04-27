#include "buffer.h"
#include <muduo/base/Mutex.h>
#include <thread>
#include <stdio.h>

using namespace muduo;
using namespace muduo::net;
Buffer    buffer_;
MutexLock mutex_;

void producerTask(){
    for(size_t i = 0; i < 1000; ++i){
        sleep(2);
        char data[2];
        size_t len = sizeof data;
        data[0] = static_cast<char>(static_cast<size_t>('A') + i % 26);
        data[1] = static_cast<char>(static_cast<size_t>('a') + i % 26);
        while(buffer_.writableBytes() < len){
             buffer_.ensureWritableBytes(len);
        }
        MutexLockGuard lock(mutex_);
        buffer_.append(data, sizeof data);
        printf("--------after produce-----------\n");
        printf("readableBytes: %d\n",  buffer_.readableBytes());
        printf("writableBytes: %d\n",  buffer_.writableBytes());
        printf("---------------------------\n");
    }
}

void consumerTask(){
    while(1){
       char data[2];
       while( buffer_.readableBytes() < sizeof data);
       MutexLockGuard lock( mutex_);
       buffer_.read(data, sizeof data);
       printf("--------consumer-----------\n");
       printf("%c%c\n", data[0], data[1]);
       printf("---------------------------\n");
   }
}

 
int main(int argc, const char *argv[])
{
    std::thread producer(producerTask); 
    std::thread consumer(consumerTask);
    producer.join();
    consumer.join();

    printf("after join\n");
    return 0;
}

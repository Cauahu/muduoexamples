#include "channel.h"
#include "eventloop.h"
#include <poll.h>

const int Channel::kNoneEvent  = 0;
const int Channel::kReadEvent  = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

void Channel::handleEvent(muduo::Timestamp receiveTime){
    if(revents_ & POLLHUP && !(revents_ & POLLIN))
        if(closeCallback_)
            closeCallback_();
    if(revents_ & (POLLERR | POLLNVAL))
        if(errorCallback_)
            errorCallback_();
    if(revents_ & (POLLIN | POLLPRI | POLLRDHUP))
        if(readCallback_){
            readCallback_(receiveTime);
        }
    if(revents_ & POLLOUT)
        if(writeCallback_)
            writeCallback_();
}


void Channel::update(){
    loop_->updateChannel(this);//update what poll focus
}


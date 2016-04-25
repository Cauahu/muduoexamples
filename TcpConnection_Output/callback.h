#ifndef __CALLBACK_H__
#define __CALLBACK_H__
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <muduo/base/Timestamp.h>
#include "buffer.h"

class TcpConnection;
typedef boost::function<void ()> TimerCallback;
typedef boost::shared_ptr<TcpConnection> TcpConnectionPtr;

typedef boost::function<void (const TcpConnectionPtr&)> ConnectionCallback;
typedef boost::function<void (const TcpConnectionPtr&,
                              Buffer*,
                              muduo::Timestamp)>MessageCallback;

typedef boost::function<void (const TcpConnectionPtr&)> CloseCallback;

#endif  /*__CALLBACK_H__*/

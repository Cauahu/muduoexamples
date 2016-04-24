#ifndef __CALLBACK_H__
#define __CALLBACK_H__
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <muduo/net/Buffer.h>
#include <muduo/base/Timestamp.h>

class TcpConnection;
typedef boost::function<void ()> TimerCallback;
typedef boost::shared_ptr<TcpConnection> TcpConnectionPtr;

typedef boost::function<void (const TcpConnectionPtr&)> ConnectionCallback;
typedef boost::function<void (const TcpConnectionPtr&,
                              const char*,
                              size_t)>MessageCallback;

typedef boost::function<void (const TcpConnectionPtr&)> CloseCallback;

#endif  /*__CALLBACK_H__*/

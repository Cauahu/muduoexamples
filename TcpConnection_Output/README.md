##TcpConnection处理输出
发送数据和接受数据最大的不同就是，socket fd 的POLLIN事件可以一直监听，待有数据可读的时候poll返回，而POLLOUT时间不能一直监听，否则它会一直返回ready， 那么就会造成busy loop。因此，对于发送数据，必须在写之前开启监听POLLOUT事件，写完后立即关闭监听该事件。


muduo实现的发送消息首先保证了线程安全性，因为它必须在IO线程内执行发送任务；其次就是它的策略也比较有趣，是先把数据一下子发送出去，如果一次没发送完，就把剩下的放到Buffer里面，等待下一个loop再发送一次。 这里之所以不连续调用两次write系统调用，是因为有经验表明，很大的概率第二次再写的时候会返回EAGAIN 错误，表明存在资源没有准备好，或者内核缓冲区已满等问题。

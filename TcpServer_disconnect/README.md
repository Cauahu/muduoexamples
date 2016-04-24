##TcpConnection之处理连接关闭事件

###关闭连接
- 主动关闭（首先调用close关闭连接的一端，active close）;
- 被动关闭（passive close）;
- 当客户端关闭连接时，socket fd 会发生可读事件，此时的POLL事件标志是(POLLIN | POLLHUP)。

###主要处理流程：

        对端关闭连接；
        -------->EventLoop::loop中的poll返回可读事件；
        -------->调用对应的Channel处理handlEvent；
        -------->Channel根据事件的类型调用其handleRead函数；
        -------->Channel的handleRead中read返回0；
        -------->调用closeCallback回调函数，该回调是由TcpConnection注册；
        -------->在TcpConnection中调用handleClose,使得Channel无效，不再监听；
        -------->在TcpServer中管理的连接map中移除连接；
        -------->最后由eventloop去执行最终的连接释放问题；
        -------->TcpConnection被析够。

###需要注意的地方

- poller管理了监听的描述符结构体数组和所有的channel，移除的时候需要两个都处理；
- TcpServer管理了当前建立的所有连接的映射map，关闭连接的时候要移除。

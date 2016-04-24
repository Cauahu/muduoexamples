##TcpServer
主要是对Acceptor的封装，用EventLoop和InetAddress初始化，用来管理建立的连接，并且通过设置回调函数，可以对不同的事件采取自定义的行为。

##TcpConnection
目前对TcpConnection只实现了三大事件中建立连接的处理。

##主要流程

    EeentLoop
        |
        |
    TcpServer  ====={ConnectionCallback, MessageCallback}
        |                       |
        |                       |
    Acceptor                    |
        |                       |
        |                       |
    TcpConnection==========={ConnectionCallback, MessageCallback}

##Acceptor的实现
之前实现按照教材实现了Reactor模型，现在需要在其基础上实现网络库。首先学习一下Accetor的实现，Acceptor的主要功能是开启一个socket fd，然后使其处于监听状态，当新连接请求到来时，接受新的连接。
在这之前，muduo还对网络编程常用的结构体和函数进行了封装，分别是：

- class Socket: 对socket fd及其基本操作如listen， accpet等的封装；

- sockets_ops.h : 对一些网络编程常用操作的封装，如创建套接字，绑定，监听，字节序转换等。把这些操作抽出来放在一起而没有在某个类中单独实现，是因为这样可以很方便的被调用。

- class InetAddress： 对struct sockaddr_in结构体的封装，系统调用中使用的参数是struct sockaddr, 但是一般程序操作的是struct sockaddr_in 结构体，这两个结构体的定义如下，

        struct sockaddr
        {
            sa_family_t  sa_family; //通常用的是AF_INET,2个字节
            char   sa_data[14];    //socket的IP地址和端口等信息，14字节   
        };
        
        struct   sockaddr_in
        {
            short int   　　　　 sin_family; 　//通常用的是AF_INET,2个字节
            unsigned short int  sin_port;      // 端口号（网络字节序）2字节   
            struct in_addr　　　 sin_addr;     //IP地址,4字节
            unsigned char   　   sin_zero[8]; //为了和struct sockaddr保持一样的长度   
        };

一般的做法是把类型、ip地址、端口填充sockaddr_in结构体，然后强制转换成sockaddr，作为参数传递给系统调用函数。

这样再来实现Acceptor就比较方便了，Acceptor包含EventLoop指针成员，这是显然的，因为它们是在同一个事件循环中被监听，这里就是把之前的fd换成了socket fd了，其次当该fd可读时，还设置了handleRead成员函数,它的功能就是来接受新的连接请求。

##两个问题
- socket fd的安全释放问题
在Acceptor::handleRead中，直接把socket fd 传递给了回调函数，这会造成fd忘记被释放，毕竟fd是系统资源，其中作者给出的解决办法是使用C++11移动语义，把新连接的fd构造为临时对象Socket， 然后转移给回调函数，这样，可以保证回调函数结束时，fd被正确释放。关于移动语义我目前理解的就是省去中间的拷贝过程，把原对象的内容都直接转移到另一个对象上去。

- socket fd耗尽的情况
muduo的一个处理办法是提前准备一个空闲的文件描述符


##两个练习

- 把测试文件改为daytime服务器；
这个只需要在建立连接时把时间发送过去即可；
- 把服务器扩充为监听两个端口；
因为一个Acceptor是由loop和监听地址InetAddress构造的，因此，如果想要监听两个端口，就需要构造两个Acceptor, 但是二者共享同一个事件循环。
- 这两个程序分别在本目录下的 test2.cc 和 test3.cc。









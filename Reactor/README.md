##Muduo Reactor 设计模式的实现
1. `Reactor是基于事件驱动的一种设计模式`，通过注册事件， 发生事件和事件回调构成，其中心思想是采用I/O 多路复用技术。
Muduo是基于Reactor的网络库，核心是一个事件循环，用于响应I/O事件和定时器。Reactor核心就是IO复用和事件分发的实现。

2. Muduo Reactor的实现由3个类组成，`EventLoop, Poller和Channel`。其中EventLoop用于整体的事件循环，它包含Poller对象和Channel* 数组，其中Poller主要用来实现IO复用这里用poll实现，一个Channel表示它关注的文件描述符,事件及其回调，`具体的实现思路如下图`， 
 - EventLoop 通过调用Poller获取当前所有活动的文件描述符；
 - Poller 把文件描述符以Channel*数组的形式返回给EventLoop；
 - EventLoop 遍历所有的该数组，找到对应的活动事件，调用回调函数。
 
                        EventLoop             Poller  ChannelA  ChannelB
                    —loop——>|                     |     |        |
                            |——————poll————————>  |     |        |
                            |                     |     |        |
                            |<———activeChannels———|     |        |
                            |                           |        |
                            |————————handleEvent———————>|        |
                            |                                    |
                            |——————————————————handleEvent——————>|           
 


3. 测试
这里通过在监听描述符中加入一个定时器描述符，监听可读事件，定时器设置为5s后启动，在回调函数中将事件循环关闭。

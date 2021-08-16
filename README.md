Libcoevent
===

Libevent is a c/c++ coroutine library on the basis of libco, it not only retains many features of libco, but also can be integrated into many existing projects,in which you used libev,libuv,libevent and so on.

###
if you want know what is libo and how to use it, Please check website https://github.com/Tencent/libco

### 简介

libcoevent是基于腾讯libco协程库而开发的集协程与异步事件为一体的库，该库可以保留libco的特性的同时还可以支持主流的异步事件库。

Libcoevent is a library integrating coroutines and asynchronous events,it developed based on Tencent's libco coroutine library. Libcoevent can retain the characteristics of libco while also supporting mainstream asynchronous event libraries.

通过对libcoevent中的配置进行调整，你可以将libco无缝整合进你现有的开发项目中，且基本不用做改动。举例来说，如果你现有的项目采用libev做为异步事件库，你只需要对libcoevent中加入USE_LIBEV配置，调用setEventLoop 函数，之后，你就可以放心作用libco带给你的协程特性了，具体使用看HOWTO.

By adjusting the macro in libcoevent, you can seamlessly integrate libco into your existing development projects, and basically do not need to make changes. For example, if your existing project uses libev as the asynchronous event library, you only need to add macro USE_LIBEV to libcoevent, call the setEventLoop() function, and then you can rest assured that you can use the coroutine features that libco brings to you. 

作者: klavien(klavien@163.com) 

PS: **该库还处在不断完善中，主要是对几个主流事件库的支持，并加入一些新特性，欢迎参与改造，并提出宝贵bug.**

The library is still in continuous improvement, mainly to support several mainstream event libraries, and add some new features, welcome to participate in the transformation, and raise valuable bugs.

### HOWTO

Libcoevent的使用比较简单。

```bash
$ cd /path/to/libcoevent
$ make
```
注意点：
因为我本地用libev做了测试，Makefile中已经被事先加了libev的预定义宏USE_LIBEV，如果你想切换成其它事件库或用libco原生的epoll_wait，可以进行修改或删除。

Because I used libev for testing locally, the pre-defined macro USE_LIBEV of libev has been added to the Makefile in advance. If you want to switch to other event libraries or use libco's native epoll_wait, you can modify or delete it.

支持的事件库及宏定义如下：

1. libco原生 --> 不加任何USE_*宏定义 (已支持)  

2. libev    --> USE_LIBEV    (已支持)  

3. libae    --> USE_LIBAE    (redis内部使用的ae事件库，已支持)  

4. libuv    --> USE_LIBUV    (暂不支持，支持中:-))  

5. libevent --> USE_LIBEVENT (暂不支持，支持中:-))  


### 最后

I do this just for fun.

一个人精力时间有限，有时间就开发点，欢迎有朋友加入，多提bug，如有问题请联系我。

One person has limited energy and time. If you have time, you can develop points. Friends are welcome to join and raise bugs. If you have any questions, please contact me.

Enjoy your corotine world！

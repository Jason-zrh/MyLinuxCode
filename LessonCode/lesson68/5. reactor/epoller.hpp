#pragma once 
#include <iostream>
#include <cerrno>
#include <string.h>
#include <sys/epoll.h>
#include "nocopy.hpp"
#include "log.hpp"

using namespace std;

extern log lg;

// 不允许拷贝构造和赋值
class Epoller : public nocopy
{    
public:
    static const int size = 128;

    // 创建epoller
    Epoller()
    { 
        _epfd = epoll_create(size);
        if(_epfd == -1)
        {
            lg(Error, "epoll_create error. errno: %d, error: %s", errno, strerror(errno));
        }
        else
        {
            lg(Info, "epoll_create success: %d", _epfd);
        }
    }
    
    int epollWait(epoll_event revents[], int num, int timeout)
    {
        // 参数: 关心哪个epoll文件描述符, 就绪后的文件描述符结构体放在哪, 就绪后最多存放的数量, timeout
        int n = epoll_wait(_epfd, revents, num, timeout); // 这里把timeout设为 -1 表示阻塞等待
        return n;
    }

    // 对epoll的rb_treee进行op操作, event是关心读或写或异常
    void epollUpdate(int op, int fd, uint32_t event)
    {
        int n = 0;
        // 删除的时候不用设置事件结构体
        if(op == EPOLL_CTL_DEL)
        {
            n = epoll_ctl(_epfd, op, fd, nullptr);
            if(n < 0)
            {
                lg(Error, "epoll_ctl delete error!");
            }
        }
        // add或mod
        else
        {
            epoll_event ev;
            ev.events = event;
            ev.data.fd = fd; // 方便后续操作得知是哪个文件描述符下的事件就绪
            n = epoll_ctl(_epfd, op, fd, &ev);
            if(n < 0)
            {
                lg(Error, "epoll_ctl op error!");
            }
        }
    }

    ~Epoller()
    {
        // 直接关闭文件描述符
        if(_epfd > 0)
        {
            close(_epfd);
        }
    }
private:
    int _epfd;
};
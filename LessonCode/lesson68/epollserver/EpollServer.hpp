#pragma once
#include <iostream>
#include <sys/epoll.h>
#include <memory>
#include "Socket.hpp"
#include "epoller.hpp"

using namespace std;

extern log lg;
static const uint16_t defaultport = 8080;
uint32_t EVENT_IN = (EPOLLIN);   // 读事件
uint32_t EVENT_OUT = (EPOLLOUT); // 写事件

class EpollServer
{
    static const int num = 64; // 就绪列表的最大存储数量

public:
    EpollServer(uint16_t port = defaultport)
        : _port(port), _listenSock_ptr(new Sock()), _epoller_ptr(new Epoller())
    {
    }

    bool Init()
    {
        _listenSock_ptr->Socket();
        _listenSock_ptr->Bind(_port);
        _listenSock_ptr->Listen();
        lg(Info, "Create listen socket: %d", _listenSock_ptr->GetFd());
        return true;
    }

    void Accepter(int fd)
    {
        string clientip;
        uint16_t clientport;
        // accept
        int n = _listenSock_ptr->Accept(&clientip, &clientport);
        if (n < 0)
        {
            lg(Warning, "accept error");
        }
        else
        {
            // 将新链接后的文件描述符添加到rb_tree
            _epoller_ptr->epollUpdate(EPOLL_CTL_ADD, n, EVENT_IN);
            lg(Info, "Get a new link, client info@ %s:%d", clientip.c_str(), clientport);
        }
    }

    void Reciever(int fd)
    {
        char buffer[1024];
        // 从文件描述符中读数据
        ssize_t n = read(fd, buffer, sizeof(buffer) - 1);
        if (n > 0)
        {
            // 成功从文件描述符中读到东西
            buffer[n - 1] = 0; // n - 1代表把回车也去掉
            cout << "Get a message: " << buffer << endl;
        }
        else if (n == 0)
        {
            // 对方文件描述符已经关闭，那这边文件描述符也要关闭
            lg(Info, "Client quit, me too. Close %d", fd);
            // 细节，当epoll_ctl时，fd必须是合法的，否则会报错，所以应该先删除rb_tree中的节点后关闭文件描述符
            _epoller_ptr->epollUpdate(EPOLL_CTL_DEL, fd, 0);// 删除的时候不关心事件
            close(fd);
        }
        else
        {
            // 读出现异常，同样关闭
            lg(Warning, "Read failed. Close %d", fd);
            _epoller_ptr->epollUpdate(EPOLL_CTL_DEL, fd, 0);
            close(fd);
        }
    }

    void Dispatcher(epoll_event revents[], int n)
    {
        // 遍历就绪队列中已经就绪的事件
        for (int i = 0; i < n; i++)
        {
            int sockfd = revents[i].data.fd;
            uint32_t event = revents[i].events;
            if (event & EVENT_IN)
            {
                // 如果是listensock
                if (sockfd == _listenSock_ptr->GetFd())
                {
                    // 建立新的链接，并把新链接添加到epoll的rb_tree中
                    Accepter(sockfd);
                }
                else
                {
                    Reciever(sockfd);
                }
            }
            else if (event & EVENT_OUT)
            { }
            else
            { }
        }
    }

    // 服务器启动
    void Start()
    {
        // 先把listenfd添加到关心列表(读事件)
        _epoller_ptr->epollUpdate(EPOLL_CTL_ADD, _listenSock_ptr->GetFd(), EVENT_IN);
        // 等待就绪队列
        epoll_event revents[num]; // ready events
        while (1)
        {
            int n = _epoller_ptr->epollWait(revents, num);
            if (n > 0)
            {
                // 有事件就绪
                Dispatcher(revents, n);
            }
            else if (n == 0)
            {
                lg(Info, "time out ...");
            }
            else
            {
                lg(Error, "epll wait error");
            }
        }
    }

    ~EpollServer()
    {
        close(_listenSock_ptr->GetFd());
    }

private:
    shared_ptr<Sock> _listenSock_ptr;
    shared_ptr<Epoller> _epoller_ptr;
    uint16_t _port;
};
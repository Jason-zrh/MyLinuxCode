#pragma once
#include <iostream>
#include <sys/epoll.h>
#include <unordered_map>
#include <functional>
#include <string.h>
#include <memory>
#include "Socket.hpp"
#include "epoller.hpp"
#include "nocopy.hpp"
#include "log.hpp"
#include "Comm.hpp"

class Connection;
class TcpServer;

uint32_t EVENT_IN = (EPOLLIN | EPOLLET); // ET模式的epoll
uint32_t EVENT_OUT = (EPOLLOUT | EPOLLET);

using namespace std;
using func_t = function<void(shared_ptr<Connection>)>;

// 给每个文件描述符都创建一个fd对象，管理它的读和写缓冲区
class Connection
{
public:
    Connection(int sock, shared_ptr<TcpServer> tcp_svr_ptr)
        : _sock(sock)
        ,_tcp_svr_ptr(tcp_svr_ptr)
    {
    }

    // 设置回调函数方法
    void SetHandler(func_t recv, func_t send, func_t except)
    {
        _recv_cb = recv;
        _send_cb = send;
        _except_cb = except;
    }

    ~Connection()
    {
    }

private:
    int _sock; // 对应的文件描述符
    shared_ptr<TcpServer> _tcp_svr_ptr;

    string _recv_buffer; // 接收缓冲区
    string _send_buffer; // 发送缓冲区
    func_t _recv_cb;     // 回调函数
    func_t _send_cb;
    func_t _except_cb;
};

static const uint16_t defaultport = 8080;

class TcpServer : public nocopy
{
    static const int num = 64;

public:
    TcpServer(uint16_t port = defaultport)
        : _port(port), _epoller_ptr(new Epoller()), _listensock_ptr(new Sock()), _quit(false)
    {
    }

    void AddConnection(int sock, uint32_t event, func_t recv, func_t send, func_t except)
    {
        // 1. 创建connection对象
        shared_ptr<Connection> new_connection = make_shared<Connection>(sock, shared_ptr<TcpServer>(this));
        new_connection->SetHandler(recv, send, except);
        // 2. 将fd和connection的映射关系添加到unordered_map中

        _connections[sock] = new_connection;
        // 3. 将fd和event设置到内核中
        _epoller_ptr->epollUpdate(EPOLL_CTL_ADD, sock, event);
    }

    bool Init()
    {
        _listensock_ptr->Socket();                 // 创建套接字
        SetNoBlockOrDie(_listensock_ptr->GetFd()); // 设置非阻塞轮询
        _listensock_ptr->Bind(_port);              // 绑定端口
        _listensock_ptr->Listen();                 // 监听
        // lg(Info, "create listen socket success: %d", _listensock_ptr->GetFd());
        // 创建connection对象，并将映射关系添加到unordered_map中
        // 将listensock和event添加到内核
        AddConnection(_listensock_ptr->GetFd(), EVENT_IN, nullptr, nullptr, nullptr);
        return true;
    }

    void Dispatcher(int timeout)
    {
        int n = _epoller_ptr->epollWait(_revs, num, timeout);
        for (int i = 0; i < n; i++)
        {
            uint32_t events = _revs[i].events;
            int sock = _revs[i].data.fd;

            // 读事件就绪
            if(events & EPOLLIN)
            {

            }
            if(events & EPOLLOUT)
            {

            }
            if(events & EPOLLERR)
            {
                
            }
        }
    }

    void Loop()
    {
        _quit = true;
        while (_quit)
        {
            Dispatcher(3000);
        }
        _quit = false;
    }

    ~TcpServer()
    {
        close(_listensock_ptr->GetFd());
    }

private:
    shared_ptr<Epoller> _epoller_ptr;                         // epoll指针
    shared_ptr<Sock> _listensock_ptr;                         // 套接字
    uint16_t _port;                                           // 端口号
    epoll_event _revs[num];                                   // epoll就绪事件队列
    unordered_map<int, shared_ptr<Connection>> _connections;  // fd与connection的映射关系
    bool _quit;                                               // 标志服务器是否启动
};
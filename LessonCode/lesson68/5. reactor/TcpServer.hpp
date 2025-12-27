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

extern log lg;
using namespace std;
using func_t = function<void(weak_ptr<Connection>)>;
using except_func = function<void(weak_ptr<Connection>)>;

// ================================ Connection对象 ================================
// 给每个文件描述符都创建一个Connection对象，管理它的读和写缓冲区
class Connection
{
public:
    Connection(int sock)
        : _sock(sock)
    {
    }

    // ##################### 设置回调函数方法 #####################
    void SetHandler(func_t recv, func_t send, func_t except)
    {
        _recv_cb = recv;
        _send_cb = send;
        _except_cb = except;
    }

    // ##################### 将接受到的信息放到接收缓冲区 #####################
    void AppendInBuffer(const string &info)
    {
        _recv_buffer += info;
    }

    void AppendOutBuffer(const string &info)
    {
        _send_buffer += info;
    }

    std::string &Inbuffer()
    {
        return _recv_buffer;
    }
    std::string &OutBuffer()
    {
        return _send_buffer;
    }

    int Getfd()
    {
        return _sock;
    }

    void SetWeakPtr(std::weak_ptr<TcpServer> tcp_server_ptr)
    {
        _tcp_svr_ptr = tcp_server_ptr;
    }

    ~Connection()
    {
    }

private:
    int _sock;           // 对应的文件描述符
    string _recv_buffer; // 接收缓冲区
    string _send_buffer; // 发送缓冲区

public:
    func_t _recv_cb; // 回调函数
    func_t _send_cb;
    except_func _except_cb;
    weak_ptr<TcpServer> _tcp_svr_ptr; // 回指到TcpServer
};

// ================================ TcpServer ================================

class TcpServer : public std::enable_shared_from_this<TcpServer>, public nocopy
{
    static const int num = 64; // epoll就绪队列最大容量

public:
    TcpServer(uint16_t port, func_t onMessage)
        : _port(port)
        , _epoller_ptr(new Epoller())
        , _listensock_ptr(new Sock())
        , _quit(false)
        , _onMessage(onMessage)
    {
    }

    // ##################### 给新的fd创建一个connection对象 #####################
    void AddConnection(int sock, uint32_t event, func_t recv, func_t send, func_t except)
    {
        // 1. 创建connection对象
        shared_ptr<Connection> new_connection(new Connection(sock));
        new_connection->SetHandler(recv, send, except);
        new_connection->SetWeakPtr(shared_from_this());
        // 2. 将fd和connection的映射关系添加到unordered_map中
        _connections[sock] = new_connection;
        // 3. 将fd和event设置到内核中
        _epoller_ptr->epollUpdate(EPOLL_CTL_ADD, sock, event);
    }

    // ##################### 服务器初始化 #####################
    bool Init()
    {
        _listensock_ptr->Socket();                 // 创建套接字
        SetNoBlockOrDie(_listensock_ptr->GetFd()); // 设置非阻塞轮询
        _listensock_ptr->Bind(_port);              // 绑定端口
        _listensock_ptr->Listen();                 // 监听
        // 创建connection对象，并将映射关系添加到unordered_map中
        // 将listensock和event添加到内核, 将关心事件设置为读事件, 回调方法应设置为建立新链接
        AddConnection(_listensock_ptr->GetFd(), EVENT_IN,
                      bind(&TcpServer::Accepter, this, placeholders::_1), nullptr, nullptr);

        return true;
    }

    // ##################### 主循环 #####################
    void Loop()
    {
        _quit = true;
        while (_quit)
        {
            // Dispatcher(3000);
            Dispatcher(-1); // 设置为阻塞等待一下
        }
        _quit = false;
    }

    // ##################### 事件派发器 #####################
    void Dispatcher(int timeout)
    {
        // 对添加到epoll模型中的事件设置等待
        int n = _epoller_ptr->epollWait(_revs, num, timeout);
        for (int i = 0; i < n; i++)
        {
            uint32_t events = _revs[i].events;
            int sock = _revs[i].data.fd;

            // 读事件就绪
            if ((events & EPOLLIN) && IsConnectionSafe(sock))
            {
                // 如果回调函数直接就调用
                if (_connections[sock]->_recv_cb)
                    _connections[sock]->_recv_cb(_connections[sock]);
            }
            // 写事件就绪
            if ((events & EPOLLOUT) && IsConnectionSafe(sock))
            {
                if (_connections[sock]->_send_cb)
                    _connections[sock]->_send_cb(_connections[sock]);
            }
            // 异常
            if ((events & EPOLLERR) && IsConnectionSafe(sock))
            {
                if (_connections[sock]->_except_cb)
                    _connections[sock]->_except_cb(_connections[sock]);
            }
        }
    }

    // ================================ 事件管理器 ================================
    // ##################### 链接管理器(与listen套接字绑定) #####################
    void Accepter(weak_ptr<Connection> conn)
    {
        auto connection = conn.lock();
        // 非阻塞循环
        while (1)
        {
            sockaddr_in peer;
            socklen_t len = sizeof(peer);
            int newfd = accept(connection->Getfd(), (sockaddr *)&peer, &len);
            // if(newfd < 0)
            // {
            //     // 建立新链接错误了
            // }

            // 设置非阻塞了，所以出现异常可能是还没就绪
            if (newfd > 0)
            {
                // 获得新的文件描述符
                // 设置非阻塞
                SetNoBlockOrDie(newfd);
                // 创建新connection对象, 关心读事件, 设置三个回调函数
                AddConnection(newfd, EVENT_IN,
                              bind(&TcpServer::Reciever, this, placeholders::_1),
                              bind(&TcpServer::Sender, this, placeholders::_1),
                              bind(&TcpServer::Expecter, this, placeholders::_1));

                // 保存链接主机的信息
                uint16_t peerport = ntohs(peer.sin_port);
                char peerip[128];
                inet_ntop(AF_INET, &peer.sin_addr.s_addr, peerip, sizeof(peerip));
                lg(Debug, "Get a new client, info [%s:%d]", peerip, peerport);
            }
            else
            {
                if (errno == EWOULDBLOCK) // 事件没就绪
                    break;
                else if (errno == EINTR) // 被信号中断
                    continue;
                else
                {
                    lg(Warning, "Accept newfd error, errno: %d, error: %s", errno, strerror(errno));
                    break;
                }
            }
        }
    }

    // ##################### 接收管理器 #####################
    void Reciever(weak_ptr<Connection> conn)
    {
        if (conn.expired())
            return;
        auto connection = conn.lock();
        // 同样是ET模式用while循环
        while (1)
        {
            char buffer[1024];
            memset(buffer, 0, sizeof(buffer));
            // 从文件描述符中读到connection的缓冲区中
            int n = recv(connection->Getfd(), buffer, sizeof(buffer) - 1, 0);
            if (n > 0)
            {
                connection->AppendInBuffer(buffer);
                // For debugging
                // buffer[n] = 0;
                // cout << "Echo# " << buffer << endl;
            }
            else if (n == 0)
            {
                // 对端文件描述符关闭
                lg(Info, "Client quit, me too");
                connection->_except_cb(connection);
                return;
            }
            else
            {
                if (errno == EWOULDBLOCK)
                    break;
                else if (errno == EINTR)
                    continue;
                else
                {
                    // 出现异常，则直接交给异常回调函数
                    connection->_except_cb(connection);
                    return;
                }
            }
        }

        // 读结束后检查接收缓冲区中数据是否读全，读全->处理，没全->继续读
        _onMessage(connection);
    }

    // ##################### 发送管理器 #####################
    void Sender(weak_ptr<Connection> conn)
    {
        /*
        写事件比较特殊
        在epoll/poll/select中，写事件基本一直是就绪的
        如果对写事件一直关心，就会导致epoll经常返回，导致CPU资源造成浪费
        结论: 对于读设置为常关心， 对于写则按需设置
        怎么处理写入？-> 直接写，如果outbuffer在写完以后还有东西则设置为关心，如果写完了则去掉对它的关心
        */
        if (conn.expired())
            return;
        auto connection = conn.lock();
        auto &outbuffer = connection->OutBuffer();
        while (1)
        {
            int n = send(connection->Getfd(), outbuffer.c_str(), outbuffer.size(), 0);
            if (n > 0)
            {
                // 发送数据，但是可能没发送全
                outbuffer.erase(0, n); // 把已经发送的数据删除
                if(outbuffer.empty())
                    break; // 发送缓冲区已经清空
            }
            else if (n == 0)
            {
                return; // 发送出问题了，缓冲区中没有数据
            }
            else
            {
                if(errno == EWOULDBLOCK) 
                    break;
                else if(errno == EINTR) 
                    continue;
                else{
                    // 出现异常直接交给异常回调
                    connection->_except_cb(connection);
                    return;
                }
            }
        }

        // 循环结束，如果发送缓冲区中还有数据，则要对它进行设置常关心，否则就不用关心
        if(outbuffer.empty())
        {
            // 没有数据
            EnableEvent(connection->Getfd(), true, false);
        }
        else
        {
            // 还有数据需要设置关心
            EnableEvent(connection->Getfd(), true, true);
        }
    }

    // ##################### 异常管理器 #####################
    void Expecter(weak_ptr<Connection> conn)
    {
        if (conn.expired())
            return;
        auto connection = conn.lock();

        // 出现异常
        lg(Warning, "Excepter hander sockfd: %d", connection->Getfd());
        // 1. 将事件从epoll模型中取消关心
        _epoller_ptr->epollUpdate(EPOLL_CTL_DEL, connection->Getfd(), 0);

        // 2. 关闭文件描述符
        lg(Debug, "close %d done...\n", connection->Getfd());
        close(connection->Getfd());

        // 3. 从unordered_map中删除
        lg(Debug, "remove %d from _connections...\n", connection->Getfd());
        _connections.erase(connection->Getfd());
    }

    // =======================================================================

    // ##################### 检查安全 #####################
    bool IsConnectionSafe(int sock)
    {
        auto iter = _connections.find(sock);
        if (iter == _connections.end())
            return false;

        return true;
    }
    ~TcpServer()
    {
        close(_listensock_ptr->GetFd());
    }


    // ##################### 设置文件关心程度 #####################
    void EnableEvent(int sock, bool readable, bool writeable)
    {
        uint32_t events = 0;
        events |= ((readable ? EPOLLIN : 0) | (writeable ? EPOLLOUT : 0) | EPOLLET);
        _epoller_ptr->epollUpdate(EPOLL_CTL_MOD, sock, events);
    }
private:
    shared_ptr<Epoller> _epoller_ptr;                        // epoll指针
    shared_ptr<Sock> _listensock_ptr;                        // 套接字
    uint16_t _port;                                          // 端口号
    epoll_event _revs[num];                                  // epoll就绪事件队列
    unordered_map<int, shared_ptr<Connection>> _connections; // fd与connection的映射关系
    bool _quit;                                              // 标志服务器是否启动
    func_t _onMessage;                                       // 处理读事件
};
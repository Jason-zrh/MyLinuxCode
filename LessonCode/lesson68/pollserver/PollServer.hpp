#pragma once
#include <iostream>
#include <unistd.h>
#include "Socket.hpp"
#include <poll.h>

extern log lg;
using namespace std;

int defaultfd = -1;
static const uint16_t defaultport = 8080;
static const int fd_max_num = 64;
// 默认事件
int non_event = 0;

class PollServer
{
public:
    PollServer(const uint16_t &port = defaultport)
    : _port(port)
    {
        // 初始化poll数组
        for(int i = 0; i < fd_max_num; i++)
        {
            _eventfds[i].fd = defaultfd;
            _eventfds[i].events = non_event;
            _eventfds[i].revents = non_event;
        }
    }

    bool Init()
    {
        // 创建套接字
        _listenSocket.Socket();
        // 绑定端口
        _listenSocket.Bind(_port);
        // 监听端口
        _listenSocket.Listen();
        return true;
    }

    // 多路复用版本(poll)
    void Start()
    {
        _eventfds[0].fd = _listenSocket.GetFd();
        // listen套接字所等待的工作就是读工作，所以直接请求监听读就可以
        _eventfds[0].events = POLLIN;
        int timeout = -1; // 3s
        while (1)
        {
            int n = poll(_eventfds, fd_max_num, timeout); // timeout 如果小于0则阻塞等待，0则轮询，大于0则等待timeout(ms)
            switch (n) 
            {
            case 0:
                // 等待超时 期间没有事件就绪
                cout << "time out" << endl;
                break;
            case -1:
                // select过程中出现错误
                cerr << "poll error" << endl;
                break;
            default:
                // 所监视的读写异常文件描述符中有一个或多个文件描述符事件就绪
                // 如果上层不处理，就会一直触发告警
                // cout << "Get a new link!" << endl;
                Dispatcher();
                break;
            }
        }
    }

    void Accepter()
    {
        // 建立新链接
        string clientip;
        uint16_t clientport;
        int sock = _listenSocket.Accept(&clientip, &clientport);
        if (sock < 0)
        {
            // 接收新链接失败
            return;
        }
        // 接收新链接成功
        lg(Info, "Accept new link: [%s:%d]", clientip.c_str(), clientport);

        // 接收到新链接以后要把新的文件描述符插入到文件描述符数组中
        int pos = 1;
        for (; pos < fd_max_num; pos++)
        {
            if (_eventfds[pos].fd != defaultfd)
                continue;
            else
                break;
        }
        // 走到这里代表找到空位或没有空位了
        if (pos == fd_max_num)
        {
            // 没有空位, 先关闭服务吧
            close(sock);
            lg(Warning, "Server full, %d close", sock);
        }
        else
        {
            // 有空位, 插入文件描述符
            _eventfds[pos].fd = sock;
            // 设置监听事件
            _eventfds[pos].events = POLLIN;
        }
    }

    void Reciever(int fd, int pos)
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
            close(fd);
            // 从文件描述符数组中掉
            _eventfds[pos].fd = defaultfd; // 其他的参数不用改，下一次设置的时候会自动覆盖
        }
        else
        {
            // 读出现异常，同样关闭
            lg(Warning, "Read failed. Close %d", fd);
            close(fd);
            _eventfds[pos].fd = defaultfd;
        }
    }

    void Dispatcher()
    {
        // 走到这里代表监视的文件描述符有就绪的了，遍历查找已经就绪的文件描述符
        for (int i = 0; i < fd_max_num; i++)
        {
            int fd = _eventfds[i].fd;
            if (fd == defaultfd)
                continue;

            if (_eventfds[i].revents & POLLIN)
            {
                // 就绪的文件描述符是listensock,  则建立新链接
                if (fd == _listenSocket.GetFd())
                {
                    Accepter();
                }
                // 就绪的是读文件描述符, 则从文件描述符中读出东西
                else
                {
                    Reciever(fd, i);
                }
            }
        }
    }

    ~PollServer()
    {
        close(_listenSocket.GetFd());
    }

private:
    Sock _listenSocket;
    uint16_t _port;
    struct pollfd _eventfds[fd_max_num];
    // int _fdArray[fd_max_num];
};
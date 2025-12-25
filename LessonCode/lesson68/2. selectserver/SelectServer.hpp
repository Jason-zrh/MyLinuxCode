#pragma once
#include <iostream>
#include <unistd.h>
#include <sys/select.h>
#include "Socket.hpp"

extern log lg;
using namespace std;

int defaultfd = -1;
static const uint16_t defaultport = 8080;
static const int fd_max_num = sizeof(fd_set) * 8;

class SelectServer
{
public:
    SelectServer(const uint16_t &port = defaultport)
        : _port(port)
    {
        // 默认将文件描述符数组全部设置为-1
        for (int i = 0; i < fd_max_num; i++)
        {
            _fdArray[i] = defaultfd;
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

    // 多路复用版本
    void Start()
    {
        int listensock = _listenSocket.GetFd();
        _fdArray[0] = listensock;
        while (1)
        {
            // 因为rfds是输入输出型参数, 每次循环的时候rfds的值都会被上一次所覆盖, 所以每次都要重新设置监听的fd
            // 每一次循环进来都要重新设置一次文件描述符数组
            for (int i = 1; i < fd_max_num; i++)
            {
                // 设置最大文件描述符
                int maxfd = _fdArray[0];
                // 创建新的描述集
                fd_set rfds;
                FD_ZERO(&rfds);
                // 将非-1的文件描述符设置到select的监听队列
                for (int i = 0; i < fd_max_num; i++) // 第一次循环
                {
                    // 默认文件描述符
                    if (_fdArray[i] == defaultfd)
                        continue;
                    else
                    {
                        FD_SET(_fdArray[i], &rfds);
                        // 如果这个文件描述符大于maxfd则进行更新
                        if (maxfd < _fdArray[i])
                        {
                            maxfd = _fdArray[i];
                        }
                    }
                }
                // 超时时长(秒， 微秒)
                timeval timeout = {2, 0};
                // select同时监视多个fd
                // 参数1: 监控的最大fd + 1 参数2, 3, 4: 读 写 异常位图 参数5: 设置超时时间用于非阻塞轮询(如果设置为nullptr则会阻塞等待)
                // int n = select(listensock + 1, &rfds, nullptr, nullptr, &timeout);
                int n = select(maxfd + 1, &rfds, nullptr, nullptr, /*&timeout*/ nullptr);

                switch (n)
                {
                case 0:
                    // 等待超时 期间没有事件就绪
                    cout << "time out" << endl;
                    break;
                case -1:
                    // select过程中出现错误
                    cerr << "Select error" << endl;
                    break;
                default:
                    // 所监视的读写异常文件描述符中有一个或多个文件描述符事件就绪
                    // 如果上层不处理，就会一直触发告警
                    // cout << "Get a new link!" << endl;
                    Dispatcher(rfds);
                    break;
                }
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
            if (_fdArray[pos] != defaultfd)
                continue;
            else
                break;
        }
        // 走到这里代表找到空位或没有空位了
        if (pos == fd_max_num)
        {
            // 没有空位, 先关闭服务吧
            close(sock);
            lg(Warning, "Select full, %d close", sock);
        }
        // 有空位, 插入文件描述符
        else
        {
            _fdArray[pos] = sock;
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
            _fdArray[pos] = defaultfd;
        }
        else
        {
            // 读出现异常，同样关闭
            lg(Warning, "Read failed. Close %d", fd);
            close(fd);
            _fdArray[pos] = defaultfd;
        }
    }

    void Dispatcher(fd_set &rfds)
    {
        // 走到这里代表监视的文件描述符有就绪的了，遍历查找已经就绪的文件描述符
        for (int i = 0; i < fd_max_num; i++)
        {
            int fd = _fdArray[i];
            if (fd == defaultfd)
                continue;

            if (FD_ISSET(fd, &rfds))
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

    ~SelectServer()
    {
        close(_listenSocket.GetFd());
    }

private:
    Sock _listenSocket;
    uint16_t _port;
    int _fdArray[fd_max_num];
};
// ====================== TCP服务器 ======================
#pragma once
// 封装套接字
#include "Socket.hpp"
#include <signal.h>
#include <functional>

extern log lg;

using namespace std;
using func_t = function<string(string& package)>;

class TcpServer
{
public:
    TcpServer(uint16_t port, func_t callback)
    :_port(port)
    ,_callback(callback)
    { }

    bool InitServer()
    { 
        _listenfd.Socket();
        _listenfd.Bind(_port);
        _listenfd.Listen();
        lg(Info, "Init Tcp Server Done ...");
        return true;
    }

    void Start()
    { 
        signal(SIGCHLD, SIG_IGN);
        signal(SIGPIPE, SIG_IGN);
        while(true)
        {
            // 客户端信息
            string clientip;
            uint16_t clientport;
            // 获取通信所用的文件操作符
            int sockfd = _listenfd.Accept(&clientip, &clientport);
            if(sockfd < 0)
                continue;
            lg(Info, "accept a new link, sockfd: %d, clientip: %s, clientport: %d", sockfd, clientip.c_str(), clientport);
            // 提供服务(多进程版)
            if(fork() == 0)
            {
                // 子进程
                _listenfd.Close(); // 关掉不需要的文件描述符
                while(true)
                {
                    // 从客户端读取内容
                    char buffer[1280];
                    int n = read(sockfd, buffer, sizeof(buffer));
                    if(n > 0)
                    {
                        buffer[n] = 0;
                        string in_buffer = buffer; // 面向字节流
                        // cout << in_buffer;
                        string info = _callback(in_buffer); // 使用回调函数完成任务
                        // if(info.empty()) continue;
                        // cout << info << endl;
                        // 将结果写回服务器
                        write(sockfd, info.c_str(), info.size());
                    }
                    else if(n == 0) break;
                    else break;
                }
                // 子进程退出则文件描述符自动关闭
                exit(0);
            }
            // 父进程
            close(sockfd);
        }
    }
    
    ~TcpServer()
    { }

private:
    uint16_t _port;
    Sock _listenfd;
    func_t _callback;
};
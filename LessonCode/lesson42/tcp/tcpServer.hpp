#pragma once
#include "log.hpp"
#include "Task.hpp"
#include "Daemon.hpp"
#include "threadPool.hpp"
#include <iostream>
#include <memory>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>          
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <netinet/in.h>

using namespace std;

log lg;

enum{
    SOCK_FAILED = 1,
    BIND_FAILED = 2,
    LISTEN_FAILED = 3
};

const int backlog = 10;
const int defaultsockfd = -1;
const string defaultip = "0.0.0.0";

class tcpServer;
class threadData
{
public:
    threadData(const int& sockfd, const string& ip, const uint16_t& port, tcpServer* t)
    :_sockfd(sockfd)
    ,_clientip(ip)
    ,_clientport(port)
    ,_tsvr(t)
    { }
    
public:
    int _sockfd;
    string _clientip;
    uint16_t _clientport;
    tcpServer* _tsvr;
};


class tcpServer
{
public: 
    tcpServer(const uint16_t& port, const string& ip = defaultip)
    :_listensock(-1)
    ,_port(port)
    ,_ip(ip)
    { }

// ========================== 服务器初始化 ============================
    void Init()
    {
        _listensock = socket(AF_INET, SOCK_STREAM, 0);
        // 创建套接字失败
        if(_listensock < 0)
        {
            lg(Fatal, "Create Socket failed: errno: %d, error: %s", errno, strerror(errno));
            exit(SOCK_FAILED);
        }
        lg(Info, "Create socket success, _listensock: %d", _listensock);

        // 防止服务器偶发性不能立即重启(tcp)
        int opt = 1;
        setsockopt(_listensock, SOL_SOCKET, SO_REUSEADDR|SO_REUSEPORT, &opt, sizeof(opt));

        // 创建结构体信息
        sockaddr_in local;
        memset(&local, 0, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(_port);
        inet_aton(_ip.c_str(), &(local.sin_addr));
        socklen_t len = sizeof(local);

        // 将设置信息绑定到内核
        int n = bind(_listensock, (const sockaddr* )&local, len);
        if(n < 0)
        {
            lg(Fatal, "Bind failed, errno: %d, error: %s", errno, strerror(errno));
            exit(BIND_FAILED);
        }
        lg(Info, "Bind socket success");

        // TCP是面向链接的，服务器一直处于等待链接到来的状态
        int m = listen(_listensock, backlog);
        if(m < 0)
        {
            lg(Fatal, "Listen failed, errno: %d, error: %s", errno, strerror(errno));
            exit(LISTEN_FAILED);
        }
        lg(Info, "Listen socket success");

    }   

    // static void* Routine(void* args)
    // {
    //     // 主线程一直在获取新的连接，不能去阻塞的等待子线程，所以子线程一创建出来就要立马detach
    //     pthread_detach(pthread_self());
    //     threadData* td = static_cast<threadData*>(args);
    //     td->_tsvr->Service(td->_sockfd, td->_clientip, td->_clientport);
    //     delete td;
    //     return nullptr;
    // }

// ========================== 服务器启动 ============================
    void Run()
    {
        Daemon();
        // 启动的同时要把线程池也启动
        threadPool<Task>::GetInstance()->Start();

        lg(Info, "Tcp Server is running");
        while(true)
        {
            sockaddr_in client;
            socklen_t len = sizeof(client);
            //1. 获取新链接
            int sockfd = accept(_listensock, (sockaddr*)&client, &len);
            if(sockfd < 0)
            {
                lg(Warning, "Accept failed, errno: %d, error: %s", errno, strerror(errno));
                continue;
            }
            
            // 获得发消息的人的信息
            uint16_t clientport = ntohs(client.sin_port);
            char clientip[32];
            inet_ntop(AF_INET, &(client.sin_addr), clientip, sizeof(clientip));

            // threadData* td = new threadData(sockfd, clientip, clientport, this);


            //2. 根据新链接进行通信 
            lg(Info, "Get a new link, sockfd: %d, client ip: %s, client port: %d", sockfd, clientip, clientport);

            // Version one(单进程版本)
            // Service(sockfd, clientip, clientport);
            // close(sockfd);


            // Version two(多进程)
            // pid_t id = fork();
            // if(id == 0)
            // {
            //     // 子进程
            //     close(_listensock);
            //     if(fork() > 0)
            //     {
            //         // 把子进程杀掉，但是保留孙子进程来完成子进程的任务
            //         exit(0);
            //     }
            //     Service(sockfd, clientip, clientport);
            //     close(sockfd);
            //     exit(0);
            // }
            // close(sockfd);
            // // 父进程
            // pid_t rid = waitpid(id, nullptr, 0); // 不能使用阻塞等待
            // (void)rid;

            // Version three(多线程版本)
            // pthread_t tid;
            // pthread_create(&tid, nullptr, Routine, td);

            // Version four(线程池版本)
            // 构建任务
            Task task(sockfd, clientip, clientport);
            // 将任务插入任务队列
            threadPool<Task>::GetInstance()->Push(task);
        }
    }


// ========================== 服务器功能函数(Version1 - 3) ============================
    // void Service(int sockfd, const string& clientip, const uint16_t& clientport)
    // {
    //     char buffer[4096];
    //     while(true)
    //     {
    //         // tcp通信方式是直接用文件操作完成读写
    //         int n = read(sockfd, buffer, sizeof(buffer));
    //         if(n > 0)
    //         {
    //             // 相当于读上来的是一个字符串
    //             buffer[n] = 0;
    //             cout << "Client say# " << buffer << endl;
    //         }
    //         else if (n == 0)
    //         {
    //             // 客户端关闭
    //             lg(Info, "[%s:%d] quit, server close: %d",clientip.c_str(), clientport, sockfd);
    //             break;
    //         }
    //         else
    //         {
    //             lg(Warning, "Read error, sockfd: %d, client ip: %s, client port: %d", sockfd, clientip.c_str(), clientport);
    //             break;
    //         }    
    //         string echo_string = "Sever echo# ";
    //         echo_string += buffer;
    //         // 再把拼接后的string发回去
    //         write(sockfd, echo_string.c_str(), echo_string.size());
    //     }
    // }

    ~tcpServer()
    { }

private:
    int _listensock;
    uint16_t _port;
    string _ip;
};

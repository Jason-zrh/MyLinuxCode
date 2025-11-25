#pragma once
#include "log.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <cstring>
#include <functional>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SIZE 1024

using namespace std;
// 与typedef string(const string&) func_t 相同意思
using func_t = function<string(const string&)>;

uint16_t defaultport = 8080;
string defaultip = "0.0.0.0";

extern log lg;

// 报错
enum
{
    SOCKET_ERR = 1,
    BIND_ERR = 2
};

void Usage(string cmd)
{
    cout << cmd << " + port[1023+]" << endl;
}

class UdpServer
{
public:
    UdpServer(const uint16_t& port = defaultport, const string& ip = defaultip)
    :_port(port)
    ,_ip(ip) 
    ,_isRunning(false)
    { }

    void Init()
    {
        // 1. 创建一个套接字
        _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        // 创建套接字失败
        if(_sockfd < 0)
        {   
            lg(Fatal, "Socket create failed, socket number: %d", _sockfd);
            exit(SOCKET_ERR);
        }
        // 创建成功
        lg(Info, "Socket create success, socket number: %d", _sockfd);
        // 2. 绑定一个端口
        struct sockaddr_in local;
        bzero(&local, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(_port);
        // local.sin_addr.s_addr = inet_addr(_ip.c_str());    // 1. string -> uint32_t 2. uint_32_t 必须是网络序列(大端序)
        local.sin_addr.s_addr = INADDR_ANY;
        int n = bind(_sockfd, (const struct sockaddr*)&local, sizeof(local));
        if(n < 0)
        {
            lg(Fatal, "Bind failed, errno: %d, err string: %s", errno, strerror(errno));
            exit(BIND_ERR);
        }
        // 绑定成功
        lg(Info, "Bind succuss");

    }

    void Run(func_t func)
    {
        // 服务器是一直挂起的
        _isRunning = true;
        while(true)
        {
            // 从UDP中读取数据
            struct sockaddr_in client;
            socklen_t len = sizeof(client);
            char inBuffer[SIZE];
            ssize_t n = recvfrom(_sockfd, inBuffer, sizeof(inBuffer) - 1, 0, (struct sockaddr*)&client, &len);
            if(n < 0)
            {
                lg(Warning, "Recv failed, errno: %d, err string: %s", errno, strerror(errno));
                continue;
            }

            inBuffer[n] = 0;

            string info = inBuffer;
            // string echo_string = "Server echo# " + info;
            // cout << echo_string << endl;
            string echo_string = func(info);

            // 发回给客户端
            ssize_t m = sendto(_sockfd, echo_string.c_str(), echo_string.size(), 0, (const sockaddr*)&client, len);
        }
    }


    ~UdpServer()
    { }
private:
    int _sockfd;     // 网络文件描述符
    uint16_t _port;  // 端口
    string _ip;      // ip 任意地址绑定填0就行
    bool _isRunning; // 判断服务器是否在运行
};
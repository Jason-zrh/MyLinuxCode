#pragma once
#include "log.hpp"
#include <iostream>
#include <memory>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>          
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

using namespace std;

log lg;

enum{
    SOCK_FAILED = 1,
    BIND_FAILED = 2
};

const int defaultsockfd = -1;
const string defaultip = "0.0.0.0";
const uint16_t defaultport = 8080;

class tcpServer
{
public: 
    tcpServer(const uint16_t& port = defaultport, const string& ip = defaultip)
    :_sockfd(-1)
    ,_port(port)
    ,_ip(ip)
    { }

    void Init()
    {
        _sockfd = socket(AF_INET, SOCK_STREAM, 0);
        // 创建套接字失败
        if(_sockfd < 0)
        {
            lg(Fatal, "Create Socket failed: errno: %d, error: %s", errno, strerror(errno));
            exit(SOCK_FAILED);
        }
        lg(Info, "Create socket success, sockfd: %d", _sockfd);

        // 创建结构体信息
        sockaddr_in local;
        memset(&local, 0, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(_port);
        inet_aton(_ip.c_str(), &(local.sin_addr));

        socklen_t len = sizeof(local);
        // 将设置信息绑定到内核
        int n = bind(_sockfd, (const sockaddr* )&local, len);
        if(n < 0)
        {
            lg(Fatal, "Bind failed, errno: %d, error: %s", errno, strerror(errno));
            exit(BIND_FAILED);
        }

        // TCP是面向链接的，服务器一直处于等待链接到来的状态
    }

    void Run()
    {

    }

    ~tcpServer()
    { }


private:
    int _sockfd;
    uint16_t _port;
    string _ip;
};
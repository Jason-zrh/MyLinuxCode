// ====================== 套接字封装 ======================
#include "log.hpp"
#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

log lg;

const int backlog = 10;

enum{
    SOCK_ERR = 1,
    BIND_ERR = 2,
    LISTEN_ERR = 3
};

class Sock
{
public:
    Sock(uint16_t port = -1)
    :_sockfd(port)
    { }

    ~Sock()
    { }

public:
    // 创建套接字
    void Socket()
    {   
        _sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if(_sockfd < 0)
        {
            lg(Fatal, "Socket err, errno: %d, error: %s", errno, strerror(errno));
            exit(SOCK_ERR);
        }
    }

    // 绑定
    void Bind(const uint16_t& port)
    {
        sockaddr_in server;
        memset(&server, 0, sizeof(server));
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = INADDR_ANY;
        server.sin_port = htons(port);
        socklen_t len = sizeof(server);
        int n = bind(_sockfd, (const sockaddr*)&server, len);
        if(n < 0)
        {
            lg(Fatal, "Bind err, errno: %d, error: %s", errno, strerror(errno));
            exit(BIND_ERR);
        }
    }

    // 监听
    void Listen()
    {
        int n = listen(_sockfd, backlog);
        if(n < 0)
        {
            lg(Fatal, "Listen err, errno: %d, error: %s", errno, strerror(errno));
            exit(LISTEN_ERR);
        }
    }

    // 获取链接 (服务器用)
    int Accept(string* clientip, uint16_t* clientport)
    {
        sockaddr_in client;
        socklen_t len = sizeof(client);
        int newfd = accept(_sockfd, (sockaddr*)&client, &len);
        if(newfd < 0)
        {
            lg(Warning, "Accept err, errno: %d, error: %s", errno, strerror(errno));
            return -1;
        }
        char ip[64];
        inet_ntop(AF_INET, &client.sin_addr, ip, sizeof(ip));
        *clientip = ip;
        *clientport = ntohs(client.sin_port);
        return newfd;
    }

    // 链接(客户端用)
    int Connect()
    {

    }

    // 关闭套接字
    void Close()
    {
        close(_sockfd);
    }


    

private:
    int _sockfd;
};



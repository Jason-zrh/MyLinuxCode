#pragma once
#include "log.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <cstring>
#include <unordered_map>
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
// using func_t = function<string(const string&, const string&, uint16_t)>;

// 默认构造传的端口
uint16_t defaultport = 8080;
// ip地址设为0会自动监听本机所有网卡
// 公网: 111.229.73.240 内网: 10.0.0.3 本地: 127.0.0.1
string defaultip = "0.0.0.0";

// 日志系统
log lg;

// 报错
enum
{
    SOCKET_ERR = 1,
    BIND_ERR = 2
};

// 服务器使用方法
void Usage(string cmd)
{
    cout << "Usage: " << endl;
    cout << cmd << " + port[1023+]" << endl;
}

// 服务器
class UdpServer
{
public:
    UdpServer(const uint16_t& port = defaultport, const string& ip = defaultip)
    :_port(port)
    ,_ip(ip) 
    ,_isRunning(false)
    { }


    // 初始化服务器
    void Init()
    {
        // 1. 创建一个套接字(本质是一个文件描述符)
        _sockfd = socket(AF_INET, SOCK_DGRAM, 0); // AF_INET 用来表明是IPV4家族的， SOCK_DGRAM代表是面向数据报的(UDP协议)
        // 创建套接字失败
        if(_sockfd < 0)
        {   
            lg(Fatal, "Socket create failed, socket number: %d", _sockfd);
            exit(SOCKET_ERR);
        }
        // 创建成功
        lg(Info, "Socket create success, socket number: %d", _sockfd);


        // 2. 绑定一个端口+ip

        // 创建对应套接字结构体, 作用是告诉内核这个socket要监视哪个ip➕端口
        struct sockaddr_in local;
        bzero(&local, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(_port); // 主机序列转化为网络序列(host to net)，端口是2字节16位整数，所以是s(short)
        // local.sin_addr.s_addr = inet_addr(_ip.c_str());    // 1. string -> uint32_t 2. uint_32_t 必须是网络序列(大端序)
        local.sin_addr.s_addr = INADDR_ANY;

        // 将创建好的结构体注册到内核
        int n = bind(_sockfd, (const struct sockaddr*)&local, sizeof(local));
        if(n < 0)
        {
            lg(Fatal, "Bind failed, errno: %d, err string: %s", errno, strerror(errno));
            exit(BIND_ERR);
        }
        // 绑定成功
        lg(Info, "Bind succuss");
    }

    void checkUser(const struct sockaddr_in& client, const string& clientip, uint16_t clientport)
    {
        auto iter = _onlineUser.find(clientip);
        if(iter == _onlineUser.end())
        {
            // 没有用户信息就添加用户
            _onlineUser[clientip] = client;
            cout << "[" << clientip << ":" << clientport << "] add to online user" << endl;
        }
        // 如果已有的话就什么都不做
    }

    void broadCast(const string& info, const string& clientip, uint16_t clientport)
    {
        for(const auto& user : _onlineUser)
        {
            string msg = "[";
            msg += clientip;
            msg += ":";
            msg += to_string(clientport);
            msg += "]# ";
            msg += info;
            socklen_t len = sizeof(user.second);
            sendto(_sockfd, msg.c_str(), msg.size(), 0, (sockaddr*)(&user.second), len);
        }
    }


    // 服务器可以从监听的端口收信息也可以向客户端返回信息
    void Run()
    {
        // 服务器是一直挂起的
        _isRunning = true;
        while(_isRunning)
        {
            // 从UDP中读取数据
            struct sockaddr_in client;        // 可以将向服务器发送数据的ip+端口结构体拿到
            socklen_t len = sizeof(client);     
            // 将接受到的信息存在缓冲区
            char inBuffer[SIZE];
            // 参数: 套接字，缓冲区，缓冲区大小，标志位，客户端的ip+port的数据，输入：结构体长度；输出：实际返回的地址结构长度
            ssize_t n = recvfrom(_sockfd, inBuffer, sizeof(inBuffer) - 1, 0, (struct sockaddr*)&client, &len); 
            if(n < 0)
            {
                lg(Warning, "Recv failed, errno: %d, err string: %s", errno, strerror(errno));
                continue;
            }

            uint16_t clientport = ntohs(client.sin_port);
            string clientip = inet_ntoa(client.sin_addr);
            // 在这里已经可以获得用户的ip和端口号了
            // 检查用户是否已经在哈希表中
            checkUser(client, clientip, clientport);

            // 将消息发给在线用户列表中所有人
            string info = inBuffer;
            broadCast(info, clientip, clientport);


            // '\0'
            // inBuffer[n] = 0;
            // string info = inBuffer;
            // // string echo_string = "Server echo# " + info;
            // // cout << echo_string << endl;

            // // 用回调函数对客户端发来的数据进行处理了
            // string echo_string = func(info, clientip, clientport);

            // // 发回给客户端
            // ssize_t m = sendto(_sockfd, echo_string.c_str(), echo_string.size(), 0, (const sockaddr*)&client, len);
        }
    }

    ~UdpServer()
    { }

private:
    int _sockfd;     // 网络文件描述符
    uint16_t _port;  // 端口
    string _ip;      // ip 任意地址绑定填0就行
    bool _isRunning; // 判断服务器是否在运行
    unordered_map<string, struct sockaddr_in> _onlineUser; // 映射ip地址和用户之间的关系
};
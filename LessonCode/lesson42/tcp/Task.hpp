#pragma once
#include <iostream>
#include <unistd.h>
#include <string>
#include "log.hpp"

using namespace std;

extern log lg;

class Task
{
public:
    Task(const int& fd, const string& ip, const uint16_t& port)
    :_sockfd(fd)
    ,_clientip(ip)
    ,_clientport(port)
    { }

    void run()
    {
        // 线程池支持的应该只是一个短服务,所以这里只完成一次任务后就立马停止
        char buffer[4096];
        // tcp通信方式是直接用文件操作完成读写
        int n = read(_sockfd, buffer, sizeof(buffer));
        if(n > 0)
        {
            // 相当于读上来的是一个字符串
            buffer[n] = 0;
            cout << "Client say# " << buffer << endl;
        }
        else
        {
            // 读的时候发生错误
            lg(Warning, "Read error, sockfd: %d, client ip: %s, client port: %d", _sockfd, _clientip.c_str(), _clientport);
            return;
        }    

        // 服务器对客户端的数据进行处理
        string echo_string = "Sever echo# ";
        echo_string += buffer;

        // 再把拼接后的string发回去
        write(_sockfd, echo_string.c_str(), echo_string.size());
        // 一次服务之后直接关闭文件描述符
        close(_sockfd);
    }

    void operator()()
    {
        run();
    }

    ~Task()
    { }
private:
    int _sockfd;
    string _clientip;
    uint16_t _clientport;
};
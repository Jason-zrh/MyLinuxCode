#pragma once
#include <iostream>
#include <unistd.h>
#include <sys/select.h>
#include "Socket.hpp"

using namespace std;

const uint16_t defaultport = 8080;

class SelectServer
{
public:
    SelectServer()
    { }

    bool Init()
    { 
        // 创建套接字
        _listensocket.Socket();
        // 绑定端口
        _listensocket.Bind(defaultport);
        // 监听端口
        _listensocket.Listen();
        return true;
    }

    // 多路复用版本
    void Start()
    {
        while(1)
        {
            // select同时监视多个fd

        }
    } 

    ~SelectServer()
    { }
private:
    Sock _listensocket;
};
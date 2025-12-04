#pragma once
#include "Socket.hpp"
#include "log.hpp"
#include <iostream>
#include <string>
#include <pthread.h>

using namespace std;

const uint16_t defaultport = 8080;

extern log lg;

class ThreadData
{
public:
    int _sockfd;
};

class HttpServer
{
public:
    HttpServer(uint16_t port = defaultport)
    :_port(port)
    { }

    void Start()
    {
        _listensock.Socket();
        _listensock.Bind(_port);
        _listensock.Listen();
        while(1)
        {
            // 获取客户端的信息
            string clientip;
            uint16_t clientport;
            // 阻塞式的等待链接
            int sockfd = _listensock.Accept(&clientip, &clientport);
            lg(Info, "Get a new connect - [%s:%d]", clientip, clientport);

            ThreadData* td = new ThreadData;
            td->_sockfd = sockfd;

            pthread_t tid;
            pthread_create(&tid, nullptr, threadRun, td);
        }
    }

    static void* threadRun(void* args)
    {
        pthread_detach(pthread_self());
        ThreadData* td = static_cast<ThreadData*>(args);
        char buffer[10240];
        ssize_t n = recv(td->_sockfd, buffer, sizeof(buffer), 0);
        if(n > 0)
        {
            buffer[n] = 0;
            cout << buffer << endl;
        }
        close(td->_sockfd);
        delete td;
        return nullptr;
    }


    ~HttpServer()
    { }

public:
    uint16_t _port;
    Sock _listensock;
};
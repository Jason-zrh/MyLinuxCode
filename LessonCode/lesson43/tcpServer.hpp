#pragma once
#include "Socket.hpp"
#include <signal.h>

using namespace std;

class TcpServer
{
public:
    TcpServer()
    { }

    bool InitServer()
    { 
        _listenfd.Socket();
        _listenfd.Bind(_port);
        _listenfd.Listen();
    }

    void Start()
    { 
        signal(SIGCHLD, SIG_IGN);
        signal(SIGPIPE, SIG_IGN);

        while(true)
        {
            string clientip;
            uint16_t clientport;
            int sockfd = _listenfd.Accept(&clientip, &clientport);
            if(sockfd < 0)
                continue;
            
            // 提供服务
            if(fork() == 0)
            {
                
            }
        }
    }
    
    ~TcpServer()
    { }

private:
    uint16_t _port;
    Sock _listenfd;
};
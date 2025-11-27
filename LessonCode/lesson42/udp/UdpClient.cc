#include <iostream>
#include <string>
#include <cstring>
#include <pthread.h>
#include <strings.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

void Usage(string cmd)
{
    cout << "Usage: " << endl;
    cout << cmd << " + Serverip + Serverport" << endl;
}


struct threadData
{
    sockaddr_in _client;
    int _sockfd;
};


void* recv_msg(void* args)
{
    threadData* td = static_cast<threadData*>(args);
    char buffer[1024];
    struct sockaddr_in temp;
    socklen_t len = sizeof(temp);
    while(true)
    {
        memset(buffer, 0, sizeof(buffer));
        // 从服务器接收消息
        ssize_t s = recvfrom(td->_sockfd, buffer, 1023, 0, (sockaddr*)&temp, &len);
        if(s > 0)
        {
            buffer[s] = 0;
            // cout << buffer << endl;
            cerr << buffer << endl;
        }
    }
    return nullptr;
}


void* send_msg(void* args)
{
    threadData* td = static_cast<threadData*>(args);
    string msg;
    socklen_t len = sizeof(td->_client);
    while(true)
    {
        // memset(&msg, 0, msg.size());
        cout << "Please Enter@ ";
        getline(cin, msg);

        // 向服务器发一个消息
        sendto(td->_sockfd, msg.c_str(), msg.size(), 0, (const sockaddr*)(&td->_client), len);
    }
    return nullptr;
}


// 将客户端改为多线程，将收消息和发消息分开
// 在客户端与服务器连接的时候必须要有有效的ip地址和相同的端口
int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        Usage(argv[0]);
        exit(0);
    }

    // 服务器ip
    string serverip = argv[1];
    // 服务端口
    uint16_t serverport = stoi(argv[2]);

    // 创建套接字
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0)
    {   
        cout << "socker error" << endl;
        return 1;
    }

    // // 构建目标服务器结构体
    // struct sockaddr_in server;
    // // 清空结构体中的内容
    // bzero(&server, sizeof(server));
    // server.sin_family = AF_INET;
    // server.sin_port = htons(serverport);
    // // ip地址被用户使用的时候一般是字符串格式因为更好看，所以要在这里进行转化
    // server.sin_addr.s_addr = inet_addr(serverip.c_str());
    // // socklen_t slen = sizeof(server);

    // 直接创建结构体对象，在结构体对象中初始化
    threadData td;
    bzero(&td._client, sizeof(td._client));
    td._client.sin_family = AF_INET;
    td._client.sin_port = htons(serverport);
    td._client.sin_addr.s_addr = inet_addr(serverip.c_str());
    td._sockfd = sockfd;

    // 创建两个线程一个收消息一个发消息
    pthread_t recv, send;
    pthread_create(&send, nullptr, send_msg, &td);
    pthread_create(&recv, nullptr, recv_msg, &td);
    
    // 主线程直接等待回收两个子进程
    pthread_join(recv, nullptr);
    pthread_join(send, nullptr);


    // client要bind吗？要的兄弟要的，只不过不需要用户去显式的bind，一般有操作系统自由选择
    // string msg;
    // char buffer[1024];
    // while(true)
    // {
    //     cout << "Please Enter@ ";
    //     getline(cin, msg);

    //     // 向服务器发一个消息
    //     sendto(sockfd, msg.c_str(), msg.size(), 0, (const sockaddr*)&server, slen);
    //     struct sockaddr_in temp;
    //     socklen_t len = sizeof(temp);
    //     // 从服务器接收消息
    //     ssize_t s = recvfrom(sockfd, buffer, 1023, 0, (sockaddr*)&temp, &len);
    //     if(s > 0)
    //     {
    //         buffer[s] = 0;
    //         cout << buffer << endl;
    //     }
    // }
    // // 关闭文件描述符(套接字)
    // close(sockfd);
    return 0;
}
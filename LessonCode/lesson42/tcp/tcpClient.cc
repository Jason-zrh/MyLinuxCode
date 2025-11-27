#include <iostream>
#include <memory>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>          
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

using namespace std;

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        cout << "Usage: " << argv[0] << " serverip serverport" << endl;
        exit(1);
    }
    string serverip = argv[1];
    uint16_t serverport = stoi(argv[2]);


    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        cout << "Socket failed" << endl;
        return 1;
    }


    // 先创建结构体对象
    sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(serverport);
    inet_pton(AF_INET, serverip.c_str(), &server.sin_addr);
    socklen_t len = sizeof(server);

    // Tcp是面向连接的，所以还需要向服务器发起连接
    int m = connect(sockfd, (const sockaddr*)&server, len);
    if(m < 0)
    {
        cout << "Connect error" << endl;
        return 2;
    }

    // 已经连接成功，可以向服务器发消息
    string msg;
    while(true)
    {
        cout << "Please Enter# ";
        getline(cin, msg);

        // 向打开的文件描述符写入
        write(sockfd, msg.c_str(), msg.size());

        char inbuffer[4096];
        int n = read(sockfd, inbuffer, sizeof(inbuffer));
        if(n > 0)
        {
            inbuffer[n] = 0;
            cout << inbuffer << endl;
        }
    }


    close(sockfd);
    return 0;
}
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

void Usage(string cmd)
{
    cout << cmd << " + Serverip + Serverport" << endl;
}

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        Usage(argv[0]);
        exit(0);
    }

    string serverip = argv[1];
    uint16_t serverport = stoi(argv[2]);

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0)
    {   
        cout << "socker error" << endl;
        return 1;
    }

    // 构建目标服务器结构体
    struct sockaddr_in server;
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(serverport);
    server.sin_addr.s_addr = inet_addr(serverip.c_str());
    socklen_t slen = sizeof(server);

    // client要bind吗？要的兄弟要的，只不过不需要用户去显式的bind，一般有操作系统自由选择
    string msg;
    char buffer[1024];
    while(true)
    {
        cout << "Please Enter@ ";
        getline(cin, msg);

        sendto(sockfd, msg.c_str(), msg.size(), 0, (const sockaddr*)&server, slen);

        struct sockaddr_in temp;
        socklen_t len = sizeof(temp);
        ssize_t s = recvfrom(sockfd, buffer, 1023, 0, (sockaddr*)&temp, &len);
        if(s > 0)
        {
            buffer[s] = 0;
            cout << buffer << endl;
        }
    }
    // 关闭文件描述符
    close(sockfd);
    return 0;
}
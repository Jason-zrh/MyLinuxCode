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
    // 先创建结构体对象
    sockaddr_in server;
    memset(&server, 0, sizeof(server));
    
    server.sin_family = AF_INET;
    server.sin_port = htons(serverport);
    inet_pton(AF_INET, serverip.c_str(), &server.sin_addr);
    socklen_t len = sizeof(server);

    while(true)
    {
        // 每次服务之后服务器会关闭对应的文件描述符，则需要重新建立连接

        // 创建套接字
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if(sockfd < 0)
        {
            cout << "Socket failed" << endl;
            return 1;
        }

        int recnt = 5; // 出现错误时重连次数
        int isReconnecting = false;
        do
        {
            // Tcp是面向连接的，所以还需要向服务器发起连接
            int m = connect(sockfd, (const sockaddr*)&server, len);
            if(m < 0)
            {
                // 重连次数-1，并将状态设为重连
                recnt--;
                isReconnecting = true;
                cout << "Connect error, reconnecting ... "<< recnt << endl;
                sleep(3);
            }
            else
            {
                // 连接成功, 直接break;
                break;
            }
            
        } while(recnt && isReconnecting);
        
        // 一直连接失败，重连次数消耗完
        if(recnt == 0)
        {
            cout << "User Offline ..." << endl;
            break;
        }
 
        // 已经连接成功，可以向服务器发消息
        string msg;
        cout << "Please Enter# ";
        getline(cin, msg);
        // 向打开的文件描述符写入(向服务器)
        int m = write(sockfd, msg.c_str(), msg.size());
        if(m < 0)
        {
            // 写操作失败
            cout << "Write failed" << endl;
            continue;
        }

        // 从服务器读回结果
        char inbuffer[4096];
        int n = read(sockfd, inbuffer, sizeof(inbuffer));
        if(n > 0)
        {
            inbuffer[n] = 0;
            cout << inbuffer << endl;
        }
        else
        {
            // 读出现错误
            cout << "Read failed" << endl;
            continue;
        }

        // 关闭对应的文件描述符
        close(sockfd);
    }
    
    return 0;
}
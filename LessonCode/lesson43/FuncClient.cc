#include "Socket.hpp"
#include "Protocol.hpp"
#include <iostream>
#include <time.h>
#include <unistd.h>

using namespace std;


int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        cout << "Usage: " << argv[0] << " + ip + port" << endl;
        return 1;
    }
    // 需要链接的服务器的IP和端口号
    string serverip = argv[1];
    uint16_t serverport = stoi(argv[2]);

    // 创建客户端的套接字
    Sock sockfd;
    sockfd.Socket();
    // 链接指定服务器
    sockfd.Connect(serverip, serverport);

    srand(time(nullptr) ^ getpid());
    string opers = "+-*/";
    int cnt = 1;

    while(cnt <= 20)
    {
        cout << "===============第" << cnt << "次测试===============" << endl;
        int x = rand()%100 + 1;
        usleep(1234);
        int y = rand()%50 + 1;
        usleep(1234);
        char op = opers[rand()%opers.size()];

        // 构建请求
        Request req(x, y, op);
        // 序列化
        string package;
        req.Serialization(&package);
        // cout << package << endl;
        // 添加报头
        package = Encode(package);
        // cout << package;
        // 向服务器中写入
        write(sockfd.GetFd(), package.c_str(), package.size());
        

        // 从服务器中读回计算结果
        char buffer[128];
        int n = read(sockfd.GetFd(), buffer, sizeof(buffer));
        if(n > 0)
        {
            buffer[n] = 0;
            // 先删除报头
            string in_buffer = buffer;
            // cout << in_buffer;
            string content;
            Decode(in_buffer, &content);
            // cout << content << endl;
            Response res;
            res.Deserialization(content);
            if(res._code == 0)
            {
                cout << x << " " << op << " "<< y << " = " << res._result << endl;
            }
            else
            {
                cout << "Error, errno: " << res._code << endl;
            }
        }
        cnt++;
        sleep(1);
    }
    return 0;
}
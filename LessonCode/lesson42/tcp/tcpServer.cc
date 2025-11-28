#include "tcpServer.hpp"

using namespace std;


int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        cout << "Usage: " << argv[0] << " port[1024+]" << endl;
        exit(0);
    }

    
    uint16_t port = stoi(argv[1]);
    unique_ptr<tcpServer> server (new tcpServer(port));
    // 先初始化服务器
    server->Init();
    // 再启动服务
    server->Run();

    return 0;
}
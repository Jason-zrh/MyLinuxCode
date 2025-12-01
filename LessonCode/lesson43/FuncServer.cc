// ================== 服务器 ==================
#include <iostream>
// 服务器
#include "tcpServer.hpp"
// 提供计算服务
#include "FuncServer.hpp"

using namespace std;

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        cout << "Usage: " << argv[0] << " + port" << endl;
        return 1;
    }

    uint16_t port = stoi(argv[1]);
    FuncServer func;
    TcpServer* tcps = new TcpServer(port, std::bind(&FuncServer::Caculator, &func, placeholders::_1));
    tcps->InitServer();
    tcps->Start();
    return 0;
}
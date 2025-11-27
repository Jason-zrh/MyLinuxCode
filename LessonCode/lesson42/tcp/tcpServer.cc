#include "tcpServer.hpp"

using namespace std;

int main()
{
    unique_ptr<tcpServer> server (new tcpServer());

    server->Init();
    server->Run();

    
    return 0;
}
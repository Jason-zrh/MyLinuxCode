#include <iostream>
#include <memory>
#include "TcpServer.hpp"

using namespace std;

int main()
{   
    unique_ptr<TcpServer> svr(new TcpServer());

    svr->Init();
    svr->Loop();

    return 0;
}
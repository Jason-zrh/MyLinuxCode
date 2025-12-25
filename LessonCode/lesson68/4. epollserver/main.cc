#include <iostream>
#include <memory>
#include "EpollServer.hpp"

using namespace std;

int main()
{
    unique_ptr<EpollServer> svr(new EpollServer());

    svr->Init();
    svr->Start();
    
    return 0;
}
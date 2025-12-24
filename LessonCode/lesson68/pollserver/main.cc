#include "PollServer.hpp"
#include <iostream>
#include <memory>

using namespace std;

int main()
{
    unique_ptr<PollServer> svr(new PollServer());
    
    svr->Init();
    svr->Start();

    return 0;
}
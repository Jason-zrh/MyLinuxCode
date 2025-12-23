#include "SelectServer.hpp"
#include <memory>


using namespace std;

int main()
{
    unique_ptr<SelectServer> svr(new SelectServer());

    svr->Init();
    svr->Start();

    return 0;
}
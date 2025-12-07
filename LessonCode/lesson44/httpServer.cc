#include <iostream>
#include <memory>
#include "httpServer.hpp"

using namespace std;

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        cout << "Usage: " << argv[0] << " + port[1023+]" << endl;
        exit(1);
    }
    const uint16_t port = stoi(argv[1]);

    // 智能指针
    unique_ptr<HttpServer> svr(new HttpServer(port));

    svr->Start();

    return 0;
}
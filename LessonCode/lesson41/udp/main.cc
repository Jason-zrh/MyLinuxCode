#include "UdpServer.hpp"
#include "log.hpp"

using namespace std;

log lg;

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        Usage(argv[0]);
        exit(0);
    }

    uint16_t port = stoi(argv[1]);

    unique_ptr<UdpServer> svr(new UdpServer(port));

    svr->Init();
    svr->Run();
    
    return 0;
}
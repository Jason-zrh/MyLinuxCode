#include <iostream>
#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <signal.h>

using namespace std;

void Usage(string proc)
{
    cout << "Usage: \n\t" << proc << " signum " << "pid" << endl;
}

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        Usage(argv[0]);
        exit(1);
    }

    int signum = stoi(argv[1]);
    pid_t pid = stoi(argv[2]);

    kill(pid, signum);
    return 0;
}
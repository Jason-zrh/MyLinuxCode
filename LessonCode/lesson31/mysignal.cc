#include <iostream>
#include <unistd.h>
#include <signal.h>

using namespace std;

void myhandler(int signo)
{
    cout << "grep signal: " << signo << endl;
    exit(1);
}

int main()
{
    signal(SIGINT, myhandler); // 信号只需要设置一次就行
    while(true)
    {
        cout << "I'm a crazy process" << endl;
        sleep(1);
    }

    return 0;
}
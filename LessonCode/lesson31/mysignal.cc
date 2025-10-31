#include <iostream>
#include <unistd.h>
#include <signal.h>

using namespace std;

void myhandler(int signo)
{
    cout << "grep signal: " << signo << endl;
    // exit(1);
}
 
int main()
{
    // signal(SIGINT, myhandler); // 信号只需要设置一次就行
    // signal(SIGQUIT, myhandler);
    for(int i = 1; i <= 31; i++)
    {
        signal(i, myhandler);
    }
    // 信号的产生与代码的运行是异步的
    while(true)
    {
        cout << "I'm a crazy process: "<< getpid() << endl;
        sleep(1);
    }

    return 0;
}
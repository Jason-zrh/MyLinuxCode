#include <iostream>
#include <signal.h>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>

using namespace std;
void PrintPending()
{
    sigset_t set;
    // 输出型参数
    sigpending(&set);
    for(int i = 31; i > 0; i--)
    {
        if(sigismember(&set, i))
            cout << "1";
        else
            cout << "0";
    }
    cout << endl;

}

void handler(int signo)
{
    PrintPending();
    cout << "catch a sigal, sig number = " << signo << endl;
}
int main()
{
    struct sigaction act, oact;
    memset(&act, 0, sizeof(act));
    memset(&oact, 0, sizeof(oact));

    act.sa_handler = handler;

    sigaction(2, &act, &oact);

    while(1)
    {
        cout << "I am a process: " << getpid() << endl;
        sleep(1);
    }

    return 0;
}
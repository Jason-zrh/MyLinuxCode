#include <iostream>
#include <unistd.h>
#include <signal.h>

using namespace std;

void PrintPending(sigset_t &pending)
{
    for(int i = 31; i >= 1; i--)
    {
        if(sigismember(&pending, i))
        {
            cout << "1";
        }
        else
        {
            cout << "0";
        }
    }
    cout << endl;
}

void handler(int signo)
{
    cout << "catch a signo " << signo << endl;
    exit(1);
}

int main()
{
    signal(2, handler);
    sigset_t bset;
    // 使用前必须先清空
    sigemptyset(&bset);
    sigaddset(&bset, 2); // 此时还没有将位图结构添加到进程字段，这里做的是数据预备
    // 先对二号信号进行屏蔽
    sigset_t oset;
    sigemptyset(&oset);
    sigprocmask(SIG_SETMASK, &bset, &oset); // 这个系统调用接口将set写入进程的位图
    // 重复打印pending
    sigset_t pending;
    int cnt = 0;
    while(true)
    {
        int n = sigpending(&pending);
        PrintPending(pending);
        cnt++;
        sleep(1);
        if(cnt == 10)
        {
            cout << "unblock signo 2" << endl;
            sigprocmask(SIG_SETMASK, &oset, nullptr);
        }
    }
    // 发送二号信号 ctrl + c
    return 0;
}

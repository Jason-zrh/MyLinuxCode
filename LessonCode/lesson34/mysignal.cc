#include <iostream>
#include <signal.h>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

// 如果是回收一批进程呢？
void handler(int signo)
{
    pid_t rid = 0;
    while((rid = waitpid(-1, nullptr, WNOHANG) > 0)) // -1代表回收任意pid的进程, 非阻塞等待，当有子进程未完成时直接跳过这次等待
    {
        // 循环回收一批进程
        cout << "catch a signal " << signo << " wait: " << rid << endl;
    }
}

int main()
{
    signal(17, handler);
    for(int i = 0; i < 10; i++)
    {
        pid_t id = fork();
        if(id == 0)
        {
            // child
            while(true)
            {
                cout << "I am child pid: " << getpid() << " ppid: " << getppid() << endl;
                sleep(5);
                break;
            }
            exit(0);
        }
    }
    
    // parent
    while(true)
    {
        cout << "I am father pid: " << getpid() << endl;
        sleep(1);
    }
}



// int flag = 0;

// void handler(int signo)
// {
//     cout << "catch a signal " << signo << endl;
//     flag = 1;
// }

// int main()
// {
//     signal(2s, handler);

//     while(!flag)
//     {
//     }

//     cout << "proccess quit normal" << endl;
//     return 0;
// }


// void PrintPending()
// {
//     sigset_t set;
//     // 输出型参数
//     sigpending(&set);
//     for(int i = 31; i > 0; i--)
//     {
//         if(sigismember(&set, i))
//             cout << "1";
//         else
//             cout << "0";
//     }
//     cout << endl;

// }

// void handler(int signo)
// {
//     PrintPending();
//     cout << "catch a sigal, sig number = " << signo << endl;
// }
// int main()
// {
//     struct sigaction act, oact;
//     memset(&act, 0, sizeof(act));
//     memset(&oact, 0, sizeof(oact));

//     act.sa_handler = handler;

//     sigaction(2, &act, &oact);

//     while(1)
//     {
//         cout << "I am a process: " << getpid() << endl;
//         sleep(1);
//     }

//     return 0;
// }
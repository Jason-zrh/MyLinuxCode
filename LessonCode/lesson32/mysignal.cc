#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

using namespace std;



int main()
{
    pid_t id = fork();
    if(id == 0)
    {
        int cnt = 500;
        // child
        while(cnt--)
        {
            cout << "Im a child process, pid: " << getpid() << endl;
            sleep(1);
        }
    }

    // father
    int status;
    pid_t rid = waitpid(id, &status, 0);
    cout << "exit status: " << WIFEXITED(status) << " exit code: " << WEXITSTATUS(status) << " code dump: " << ((status >> 7) & 1) << endl;
}




// void handler(int signo)
// {
//     cout << "catch signal: " << signo << endl;
//     // exit(1);   
// }


// int main()
// {
//     signal(SIGALRM, handler);
//     int n = alarm(5);
//     while(1)
//     {
//         cout << "proc is running ..." << endl;
//         sleep(1);
//     }

//     // signal(SIGFPE, handler);
//     // int a = 10;
//     // int b = 0;
//     // int c = a / b;
//     // cout << c << endl;
//     return 0;
    
// }
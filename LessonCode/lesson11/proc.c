#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <error.h> 

int main()
{
    printf("Begin: 我是一个进程, pid: %d, ppid: %d\n", getpid(), getppid());

    pid_t id = fork();
    if(id == 0)
    {
        // 子进程
        while(1)
        {
             printf("我是一个子进程: pid: %d, ppid: %d\n", getpid(), getppid());
             sleep(1);
        }
    }
    else if(id > 0)
    {
        // 父进程
        while(1)
        {
             printf("我是一个父进程: pid: %d, ppid: %d\n", getpid(), getppid());
             sleep(1);
        }
    }
    else 
    {
        //error
        perror("fork");
        return 1;
    }


    /*
    while(1)
    {
        printf("I'm a process, my PID is: %d, my PPID is: %d\n", getpid(), getppid());
        sleep(1);
    }
    */ 

    return 0;
}

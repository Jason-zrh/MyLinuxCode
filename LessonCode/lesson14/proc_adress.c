#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int globle_val = 100;

int main()
{
    pid_t id = fork();
    
    if(id == 0)
    {
        // 子进程
        int cnt = 5;
        while(1)
        {
            printf("I am a child, my pid: %d, ppid: %d, globle_val: %d, &globle_val: %p\n", getpid(), getppid(), globle_val, &globle_val);
            sleep(1);
            if(cnt)
            {
                cnt--;
            }
            else 
            {
                globle_val = 200;
                printf("Change globle_val 100 -> 200\n");
                cnt--;
            }
        }
    }
    else 
    {
        //父进程
        while(1)
        {
            printf("I am a child, my pid: %d, ppid: %d, globle_val: %d, &globle_val: %p\n", getpid(), getppid(), globle_val, &globle_val);
            sleep(1);
        }
    }
    return 0;
}

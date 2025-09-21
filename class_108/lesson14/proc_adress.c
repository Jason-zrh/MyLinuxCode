#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int gal_val = 100;

int main()
{
    pid_t id = fork();
    
    if(id == 0)
    {
        // 子进程
        int cnt = 5;
        while(1)
        {
            printf("I am a child, my pid: %d, ppid: %d, gal_val: %d, &gal_val: %p\n", getpid(), getppid(), gal_val, &gal_val);
            sleep(1);
            if(cnt)
            {
                cnt--;
            }
            else 
            {
                gal_val = 200;
                printf("Change gal_val 100 -> 200\n");
                cnt--;
            }
        }
    }
    else 
    {
        //父进程
        while(1)
        {
            printf("I am a parent, my pid: %d, ppid: %d, gal_val: %d, &gal_val: %p\n", getpid(), getppid(), gal_val, &gal_val);
            sleep(1);
        }
    }
    return 0;
}

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    // 测试2环境变量导入处
    // putenv("LYT = 5201314");
    
    // 测试3手动导入环境变量envrion
    //extern char** environ;
    
    // 测试4导入自己想创建的环境变量
    char* const myenv[] = {
        "ZXP = 520",
        "LLL = 1314",
        NULL
    }; 
    pid_t id = fork();
    if(id == 0)
    {
        // 子进程
        // printf("before: I am a process, pid: %d, ppid: %d\n", getpid(), getppid());
        // 这类方法的标准写法
        //execl("/usr/bin/ls", "ls", "-a", "-l", NULL);
        //自带环境变量的接口--环境变量具有全局属性
        //execlp("ls", "ls", "-a", "-l", NULL);
        //char* const myargv[] = {
        //    "ls",
        //    "-a",
        //    "-l",
        //    NULL 
        //};
        //execv("/usr/bin/ls", myargv);
       

        // 运行自己的程序
        // execl("./otherExe", "otherExe", NULL);
        // printf("after: I am a process, pid: %d, ppid: %d\n", getpid(), getppid());
        
        // 测试命令行参数和环境变量导入
        
        // 测试1: 当进程的pcb被加载到cpu时，会自动导入环境变量，而子进程会继承父进程的环境变量
        // printf("I am a newprocess: pid: %d, ppid: %d\n", getpid(), getppid());
        // execl("./otherExe", "otherExe", "-a", "-l", "-w", NULL); // 这句代码的意义就是执行otherExe并把后面的参数列表也导入新进程的main函数中
                                                                 // 被自己蠢笑了，一定要记得在参数结束后面加NULL !!!!!!!
                                                                 
        // 测试2: 使用putenv()观察环境变量的继承
        // printf("I am a newprocess: pid: %d, ppid: %d\n", getpid(), getppid());
        // execl("./otherExe", "otherExe", "-a", "-l", "-w", NULL); 
        // 24:LYT = 5201314 测试成功
        
        // 测试3: 手动导入envrion环境变量
        // printf("I am a newprocess: pid: %d, ppid: %d\n", getpid(), getppid());
        // execle("./otherExe", "otherExe", "-a", "-l", "-w", NULL, environ); 
        
        // 测试4: 导入自己的环境变量
        printf("I am a newprocess: pid: %d, ppid: %d\n", getpid(), getppid());
        execle("./otherExe", "otherExe", "-a", "-b", "-c", NULL, myenv);
    }

    // 父进程
    pid_t ret = waitpid(id, NULL, 0);
    if(ret == id)
    {
        printf("wait success: father pid: %d, ret pid: %d\n", getpid(), ret);
    }
    return 0;
}

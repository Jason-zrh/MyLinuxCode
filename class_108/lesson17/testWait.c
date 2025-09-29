#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>


#define TASK_NUM 10

// 这是定义一个函数指针，void是返回值类型，task_t是别名， ()中应为参数类型
typedef void(*task_t)();
task_t tasks[TASK_NUM];

void taskPrintLog()
{
    printf("这是一个打印日志的进程, pid: %d\n", getpid());
}

void taskGraph()
{
    printf("这是一个创建图形化界面的进程, pid: %d\n", getpid());
}

void taskDetect()
{
    printf("这是一个检测运行状态的进程, pidw %d\n", getpid());
}

// 初始化任务列表
void InitTasks()
{
    for(int i = 0; i < TASK_NUM; i++)
    {
        tasks[i] = NULL;
    }
}

int AddTasks(task_t task)
{
    // 添加任务先找位置
    int pos = 0;
    for(pos = 0; pos < TASK_NUM; pos++)
    {
        if(tasks[pos] == NULL)
        {
            break;
        }
    }
    if(pos == TASK_NUM)
    {
        return -1;
    }
    else 
    {
        tasks[pos] = task;
        return 0;
    }
}

void ExectueTask()
{
    for(int i = 0; i < TASK_NUM; i++)
    {
        if(tasks[i] == NULL)
        {
            continue;
        }
        tasks[i]();
    }
}

int main()
{
    // 初始化任务列表
    InitTasks();
    AddTasks(taskGraph);
    AddTasks(taskDetect);
    AddTasks(taskPrintLog);

    pid_t id = fork();
    if(id == 0)
    {
        // 子进程
        int cnt = 10;
        while(cnt--)
        {
            printf("I am a child: pid: %d, ppid: %d\n", getpid(), getppid());
            sleep(1);
        }
        exit(1);
        
    }
    else if(id > 0) 
    {
        // 父进程
        //非阻塞轮询，一直循环问，直到子进程结束
        while(1)
        {
            // 等待子进程，防止内存泄漏
            int status = 0; // 这里status是为了使用waitpid()的输出型参数
            //pid_t ret = waitpid(id, &status, 0);

            pid_t ret = waitpid(id, &status, WNOHANG);// 非阻塞轮询
            // 下面操作还可以用宏操作替换
            //if(ret == id)
            //{
            //    // 0x7F, 0xFF
            //    printf("Wait success: %d, sig code: %d, exit code: %d\n", ret, status&0x7F, (status>>8)&0xFF);
            //}
            
            // ret > 0 表明子进程结束
            if(ret > 0)
            {
                // 宏操作判断进程是否出现异常
                // 正常退出返回真，否则则返回假
                if(WIFEXITED(status))
                {
                    printf("进程正常退出，退出码为: %d\n", WEXITSTATUS(status));
                }
                else 
                {
                    printf("进程退出异常!\n");
                }
                break;
            }
            // ret < 0 表明发生错误
            else if(ret < 0)
            {
                printf("waited failed\n");
                break;
            }
            // ret == 0 表明子进程还没结束
            else 
            {   
                // 在这里可以继续完成父进程一些小任务等待子进程结束
                ExectueTask();
                sleep(1);
            }
        }
    }
    else 
    {
        //发现错误
        perror("Bad fork\n");
        exit(-1);
    }
    
    return 0;
}

//int main()
//{
//    pid_t id = fork();
//    if(id == 0)
//    {
//        // 子进程
//        int cnt = 10;
//        while(cnt--)
//        {
//            printf("I am a child: pid: %d, ppid: %d\n", getpid(), getppid());
//            sleep(1);
//        }
//        exit(1);
//        
//    }
//    else if(id > 0) 
//    {
//        // 父进程
//        int cnt = 5;
//        while(cnt--)
//        {
//            printf("I am a parent: pid: %d, ppid: %d\n", getpid(), getppid());
//            sleep(1);
//        }
//    }
//    else 
//    {
//        //发现错误
//        perror("Bad fork\n");
//        exit(-1);
//    }
//    
//    //非阻塞轮询，一直循环问，直到子进程结束
//    while(1)
//    {
//        // 等待子进程，防止内存泄漏
//        int status = 0; // 这里status是为了使用waitpid()的输出型参数
//        //pid_t ret = waitpid(id, &status, 0);
//
//        pid_t ret = waitpid(id, &status, WNOHANG);// 非阻塞轮询
//        // 下面操作还可以用宏操作替换
//        //if(ret == id)
//        //{
//        //    // 0x7F, 0xFF
//        //    printf("Wait success: %d, sig code: %d, exit code: %d\n", ret, status&0x7F, (status>>8)&0xFF);
//        //}
//        
//        // ret > 0 表明子进程结束
//        if(ret > 0)
//        {
//            // 宏操作判断进程是否出现异常
//            // 正常退出返回真，否则则返回假
//            if(WIFEXITED(status))
//            {
//                printf("进程正常退出，退出码为: %d\n", WEXITSTATUS(status));
//            }
//            else 
//            {
//                printf("进程退出异常!\n");
//            }
//            break;
//        }
//        // ret < 0 表明发生错误
//        else if(ret < 0)
//        {
//            printf("waited failed\n");
//            break;
//        }
//        // ret == 0 表明子进程还没结束
//        else 
//        {   
//            printf("子进程还没结束，再等一下!\n");
//            sleep(1);
//        }
//    }
//    return 0;
//}

//int main()
//{
//    pid_t id = fork();
//    if(id < 0)
//    {
//        perror("Bad fork");
//        return 1;
//    }
//    else if(id == 0)
//    {
//        int cnt = 5;
//        while(cnt)
//        {
//            printf("I am a child: pid: %d, ppid: %d, cnt: %d\n", getpid(), getppid(), cnt);
//            sleep(1);
//            cnt--;
//        }
//        exit(0);
//    }
//    else 
//    {
//        int cnt = 10;
//        while(cnt--)
//        {
//            printf("I am a father: pid: %d, ppid: %d\n", getpid(), getppid());
//            sleep(1);
//        }
//    }
//    // 目前为止，进程等待是必须的
//    // wait是等待任意一个子进程
//    pid_t ret = wait(NULL);
//    if(ret == id)
//    {
//        printf("wait success: %d\n", id);
//    }
//    sleep(5);
//    return 0;
//}



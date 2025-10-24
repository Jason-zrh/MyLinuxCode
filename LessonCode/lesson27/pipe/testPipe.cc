#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <string>
#include <cstdio>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstring>

using namespace std;
// 这里是数组下标
#define N 2
#define NUM 1024

void Writer(int wfd)
{
    string s = "Hello I am a Child";
    pid_t self = getpid();
    int number = 0;
    char buffer[NUM];

    while(true)
    {
        sleep(1);
        // 构建发送字符串
        buffer[0] = 0;
        snprintf(buffer, sizeof(buffer), "%s-%d-%d", s.c_str(), self, number++);
        // 写给父进程
        write(wfd, buffer, strlen(buffer));
    }
}

void Reader(int rfd)
{
    char buffer[NUM];
    int cnt = 5;
    while(cnt--)
    {
        buffer[0] = 0;
        ssize_t n = read(rfd, buffer, sizeof(buffer));
        if(n > 0)
        {
            buffer[n] = 0; // '\0'当作字符串来用

            cout << "father get a msg[" << getpid() << "]: " << buffer << endl;
        }
        else if(n == 0)
        {
            cout << "father read file done!" << endl;
            break;
        }
        else
            break;
    }
}


int main()
{
    // 1. 建立通信信道
    int pipefd[N] = {0};
    int n = pipe(pipefd);
    if(n < 0) return 1;  // 创建管道失败
    // cout << "fd[0]:" << fdarray[0] << " , fd[1]:" << fdarray[1] << endl;
    pid_t id = fork();
    if(id < 0) 
    {
        return 2; // 创建子进程失败
    }
    // 子进程写入，父进程读取
    if(id == 0)
    {
        // 2. 进程间通信
        close(pipefd[0]); // 关闭读
        // IPC code
        // 子进程一直写
        Writer(pipefd[1]);
        close(pipefd[1]); 
        exit(0);
    }

    close(pipefd[1]);  // 关闭写
    // IPC code
    Reader(pipefd[0]);
    // 读了5秒后关掉读端，观察子进程状态
    close(pipefd[0]);
    cout << "Father close Read: " << pipefd[0] << endl;
    sleep(5);

    // 等待回收子进程
    int status = 0;
    pid_t rid = waitpid(id, &status, 0);
    if(rid < 0) return 3;// 进程回收失败

    cout << "回收子进程成功, 返回状态: " << WIFEXITED(status) << ", 进程退出信号: " << WTERMSIG(status) << endl;
    return 0;
}
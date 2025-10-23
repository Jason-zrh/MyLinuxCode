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
        // 构建发送字符串
        buffer[0] = 0;
        snprintf(buffer, sizeof(buffer), "%s-%d-%d", s.c_str(), self, number++);
        // 写给父进程
        write(wfd, buffer, strlen(buffer));
        sleep(1);
    }
}

void Reader(int rfd)
{
    char buffer[NUM];
    buffer[0] = 0;
    while(true)
    {
        ssize_t n = read(rfd, buffer, sizeof(buffer));
        if(n > 0)
        {
            buffer[n] = 0; // '\0'当作字符串来用

            cout << "father get a msg[" << getpid() << "]: " << buffer << endl;
        }
        
    }
}


int main()
{
    int pipefd[N] = {0};
    int n = pipe(pipefd);
    if(n < 0)
        return 1;
    // cout << "fd[0]:" << fdarray[0] << " , fd[1]:" << fdarray[1] << endl;
    pid_t id = fork();
    if(id < 0) 
    {
        return 2;
    }
    if(id == 0)
    {
        // child
        // 子进程写入，父进程读取
        close(pipefd[0]);  // 关闭读
        // IPC code
        Writer(pipefd[1]);

        close(pipefd[1]); 
        exit(0);
    }
    // father
    close(pipefd[1]);  // 关闭写

    // IPC code
    Reader(pipefd[0]);
    
    // 等待回收子进程
    pid_t rid = waitpid(id, nullptr, 0);
    if(rid < 0)
        return 3;

    close(pipefd[0]);
    return 0;
}
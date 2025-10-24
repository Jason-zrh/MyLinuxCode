// hpp 声明和定义写在一起
#pragma once
#include <iostream>
#include <vector>
#include <unistd.h>
#include <sys/types.h>  
#include <sys/types.h>
#include <string.h>

using namespace std;

// 进程数
const int processnum = 5;


// 父进程要管理一批进程 -- 先描述后组织
class channel
{
public:
    channel(int cmdfd, pid_t worker, const string& processname)
    :_cmdfd(cmdfd)
    ,_worker(worker)
    ,_processname(processname)
    { }

public:
    int _cmdfd;          // 父进程要进行写入的文件操作符
    pid_t _worker;       // 控制的子进程的pid
    string _processname; // 进程的名字
};

void worker()
{
    while(1)
    {
        sleep(1);
    }
}

void InitProcess(vector<channel>* channels)
{
    // 创建一批子进程
    for(int i = 0; i < processnum; i++)
    {
        // 每创建一个子进程都要创建一个管道
        int pipefd[2] = {0};
        int n = pipe(pipefd);
        
        pid_t id = fork();
        if(id == 0)
        {
            // child
            close(pipefd[1]); // 关闭子进程写
            worker();
            exit(0);
        }
        // 父进程写入，子进程读取
        close(pipefd[0]); // 关闭父进程读

        // 把新创建的子进程加入管理vector中
        string name = "process - " + to_string(i);
        channels->push_back(channel(pipefd[1], id, name));
    }
}

void Debug(const vector<channel>& channels)
{
    // Test
    for(auto& iter : channels)
    {
        cout << "cmdfd: " << iter._cmdfd << " " << "PID: " << iter._worker << " " << "Processname: " << iter._processname << " " << endl;
    }
}
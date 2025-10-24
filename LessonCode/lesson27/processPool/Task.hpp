// hpp 声明和定义写在一起
#pragma once
#include <iostream>
#include <vector>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>  
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>


using namespace std;

// 进程数
const int processnum = 5;

// 这句代码的意义是创建定义一个函数指针，函数返回值是void，无参数
typedef void (*task_t)();

// 定义一批任务
void task_1()
{
    cout << "Task 1: 加载The Legend Of Zelda" << endl;
}

void task_2()
{
    cout << "Task 2: 给用户发放滑翔伞" << endl;
}

void task_3()
{
    cout << "Task 3: 给用户增长生命上限" << endl;
}

void task_4()
{
    cout << "Task 4: 给用户增长精力上限" << endl;
}

// 用vector管理这些任务
vector<task_t> tasks;

// 装载任务
void LoadTasks()
{
    tasks.push_back(&task_1);
    tasks.push_back(&task_2);
    tasks.push_back(&task_3);
    tasks.push_back(&task_4);
}


// 父进程要管理一批进程 -- 先描述后组织
class channel
{
public:
    channel(int cmdfd, pid_t worker, const string& processname)
    :_cmdfd(cmdfd)
    ,_workerpid(worker)
    ,_processname(processname)
    { }

public:
    int _cmdfd;          // 父进程要进行写入的文件操作符
    pid_t _workerpid;       // 控制的子进程的pid
    string _processname; // 进程的名字
};

void worker()
{
    while(true)
    {
        // 从文件描述符0中读任务码
        int cmdcode;
        // 输出型参数，直接读出任务码
        int n = read(0, &cmdcode, sizeof(int));
        // n等于0代表读到管道文件结尾
        if(n == 0)
            break;
        if(n > 0)
        {
            // 执行任务
            tasks[cmdcode]();
            cout << "worker say@ mypid is " << getpid() << " recieve cmdcode: " << cmdcode << endl;
        }
    }
}

void InitProcess(vector<channel>* channels)
{
    // 创建一批子进程
    for(int i = 1; i <= processnum; i++)
    {
        // 每创建一个子进程都要创建一个管道
        int pipefd[2] = {0};
        int n = pipe(pipefd);
        
        pid_t id = fork();
        if(id == 0)
        {
            // child
            // 关闭子进程写
            close(pipefd[1]); 
            // 将管道的读端直接重定向到子进程的stdin
            dup2(pipefd[0], 0);
            // dup2之后记得关掉原fd，防止文件描述符泄漏
            close(pipefd[0]); 

            // 子进程在执行任务的时候直接在stdin中读任务就行
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

void controlworker(const vector<channel>& channels)
{
    // 选择进程
    int processpos = 0;
    // 发放10次任务
    for(int i = 1; i <= 10; i++)
    {
        // 随机选择任务
        int cmdcode = rand() % tasks.size();
        // 用channel数组管理的channel，访问只需要确定下标
        write(channels[processpos]._cmdfd, &cmdcode, sizeof(int));
        cout << "father say: " << " send cmdcode : " << cmdcode << " woker pid is " 
             << channels[processpos]._workerpid << " worker processname " 
             << channels[processpos]._processname << endl;

        // 轮询
        processpos = (processpos + 1) % channels.size();
        sleep(1);
    }
}

// 使用管道通信的特性，当写端被关闭时，子进程会读到0，代表读到管道结尾
void QuitProcess(const vector<channel>& channels)
{
    for(const auto& ch : channels) close(ch._cmdfd);
    // 父进程等待回收子进程
    for(const auto& ch : channels) waitpid(ch._workerpid, nullptr, 0);
    cout << "子进程全部被父进程成功回收" << endl;
}
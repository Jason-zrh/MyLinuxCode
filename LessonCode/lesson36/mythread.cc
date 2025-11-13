#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <string>

using namespace std;

class Request
{
public:
    Request(int start, int end, const string& name)
    :_start(start)
    ,_end(end)
    ,_threadname(name)
    { }

public:
    int _start;
    int _end;
    const string _threadname;
};

class Response
{
public:
    Response(int result = 0, int exitcode = 0)
    :_result(result)
    ,_exitcode(exitcode)
    { }
public:
    int _result;
    int _exitcode;
};


void* sumCount(void* args)
{
    Request* rq = static_cast<Request*>(args);
    Response* ret = new Response;
    for(int i = rq->_start; i <= rq->_end; i++)
    {
        cout << "\r" << rq->_threadname << " is caculate: " << i << "%" << flush;
        ret->_result += i;
        usleep(100000);
    }
    cout << endl;
    delete rq;
    return ret;
}

int main()
{
    // 创建任务类
    Request* rq = new Request(1, 100, "Thread caculate");
    // 在新线程中要执行加和
    pthread_t tid;
    pthread_create(&tid, nullptr, sumCount, rq);

    // 结果类
    void* ret;
    pthread_join(tid, &ret);
    Response* rsp = static_cast<Response*>(ret);
    cout << "The sum is: " << rsp->_result << " exitcode: " << rsp->_exitcode << endl;
    
    return 0;
}



// 证明一个函数可以被多个执行流同时使用
// void show(const string& name)
// {
//     cout << name << "say#" << "hello thread" << endl;
// }

// // 新线程(第二个执行流)
// void* threadRoutine(void* args)
// {
//     int cnt = 5;
//     while(cnt--)
//     {
//         cout << "new thread: " << getpid() << endl;
//         // show("[new thread]");
//         sleep(1);
//     }

//     // exit(11); // exit的作用是终止进程，不能用于线程终止

//     // 运行到这里线程结束
//     return (void*)10086; // 这里返回值是被直接看做一个整数的
// }

// int main()
// {
//     pthread_t tid;
//     pthread_create(&tid, nullptr, threadRoutine, nullptr);

//     sleep(1);

//     // 线程的另一种终止方式
//     // pthread_cancel(tid); // pthread_cancel会直接把返回值设为THREAD_CANCEL，这个宏的定义值就是-1


//     // 第一个执行流
//     // while(1)
//     // {
//     //     show("[main thread]");
//     //     // cout << "main thread: " << getpid() << endl;
//     //     sleep(1);
//     // }

//     // 进行线程等待, 第二个参数是返回值(输出型参数)
//     void* reval;
//     pthread_join(tid, &reval);

//     sleep(1);
//     cout << "Thread wait success, ret: "<< (long long int)reval << endl;
//     return 0;
// }
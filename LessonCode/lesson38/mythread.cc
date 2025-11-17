// 线程的互斥 
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <vector>
#include <cstring>

using namespace std;

int tickets = 3000;

class threadData
{ 
public:
    threadData(int number, pthread_mutex_t* mutex)
    :_threadName("Thread-" + to_string(number))
    ,_lock(mutex)
    { }

public:   
    string _threadName;
    pthread_mutex_t* _lock;
};

void* getTicket(void* args)
{
    threadData* td = static_cast<threadData*>(args);

    while(true)
    {
        pthread_mutex_lock(td->_lock);
        if(tickets > 0)
        {
            // 模拟抢票过程
            usleep(1000);
            printf("%s get a ticket: %d\n", td->_threadName.c_str(), tickets);
            tickets--;
            pthread_mutex_unlock(td->_lock);
        }
        else
        {
            pthread_mutex_unlock(td->_lock);
            break;
        }
        usleep(15);
    }

    printf("%s quit ...\n", td->_threadName.c_str());
    return nullptr;
}

int main()
{
    // 定义一把互斥锁，在main函数的栈上开辟的空间
    pthread_mutex_t lock;
    pthread_mutex_init(&lock, nullptr);

    vector<pthread_t> tids;
    vector<threadData* > tds;

    // 创建三个线程模拟抢票操作
    for(int i = 1; i <= 3; i++)
    {
        pthread_t tid;
        threadData* td = new threadData(i, &lock);
        tds.push_back(td);
        pthread_create(&tid, nullptr, getTicket, tds[i - 1]);
        tids.push_back(tid);
    }


    for(auto e : tids)
    {
        pthread_join(e, nullptr);
    }
    for(auto v : tds)
    {
        delete v;
    }


    pthread_mutex_destroy(&lock);
    return 0;
}






















// 线程的原生库的使用

// #include <iostream>
// #include <unistd.h>
// #include <pthread.h>
// #include <vector>
// #include <cstring>


// using namespace std;

// #define NUM 10

// void* threadRoutine(void* args)
// {
//     int cnt = 5;
//     while(cnt--)
//     {
//         printf("I'm new thread, tid: 0x%x\n", pthread_self());
//         sleep(1);
//     }

//     return nullptr;
// }

// int main()
// {
//     vector<pthread_t> tids;
//     for(int i = 0; i < 3; i++)
//     {
//         pthread_t tid;
//         pthread_create(&tid, nullptr, threadRoutine, nullptr);
//         tids.push_back(tid);
//     }


//     for(int i = 0; i < tids.size(); i++)
//     {
//         pthread_detach(tids[i]);
//     }

//     for(int i = 0; i < tids.size(); i++)
//     {
//         int n = pthread_join(tids[i], nullptr);
//         printf("n: %d, Thread id: 0x%x, why: %s\n", n, tids[i], strerror(n));
//     }

//     return 0;
// }


// class threadData
// {
// public:
//     threadData(string threadname, string tid)
//     :_threadname(threadname)
//     ,_threadid(tid)
//     { }
// public:
//     string _threadname;
//     string _threadid;
// };



// void* threadRoutine(void* args)
// {
//     threadData* td = static_cast<threadData*>(args);
//     int i = 10;
//     while(i--)
//     {
//         cout << "My pid: " << getpid() << " I'm: " << td->_threadname << " tid: " << td->_threadid << endl;
//         sleep(1);
//     }
//     delete td;
//     return nullptr;
// }

// int main()
// {
//     vector<pthread_t> tids;
//     for(int i = 0; i < NUM; i++)
//     {
//         pthread_t tid;
//         char buffer[128];
//         snprintf(buffer, sizeof(buffer), "0x%x", tid);
//         threadData* td = new threadData(("Thread-" + to_string(i)), buffer);
//         pthread_create(&tid, nullptr, threadRoutine, td);
//         tids.push_back(tid);
//         sleep(1);
//     }


//     for(int i = 0; i < tids.size(); i++)
//     {
//         pthread_join(tids[i], nullptr);
//     }
//     return 0;
// }
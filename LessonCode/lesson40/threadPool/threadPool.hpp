#pragma once
#include <iostream>
#include <queue>
#include <string>
#include <vector>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

using namespace std;

struct threadInfo
{
    pthread_t _tid;
    string _threadName;
};

static const int defaultNum = 5;
template<class T>
class threadPool
{
public:
    void Lock()
    {
        pthread_mutex_lock(&_mutex);
    }

    void unLock()
    {
        pthread_mutex_unlock(&_mutex);
    }

    // 休眠一个进程
    void threadSleep()
    {
        pthread_cond_wait(&_cond, &_mutex);
    }

    // 唤醒一个线程
    void wakeUp()
    {
        pthread_cond_signal(&_cond);
    }

    bool isQueueEmpty()
    {
        return _tasks.empty();
    }

    string getThreadName(pthread_t tid)
    {
        for(auto ti : _threads)
        {
            if(ti._tid == tid)
            {
                return ti._threadName;
            }
        }
        return "None";
    }
public:
    threadPool(int num = defaultNum) // 线程池内线程的数量
    :_threads(num)
    { 
        pthread_mutex_init(&_mutex, nullptr);
        pthread_cond_init(&_cond, nullptr);
    }

    // 类内非静态成员的参数列表里还有一个隐藏的threadPool this 这个参数，只有变成静态的函数才可以
    static void* handleTask(void* args)
    {
        threadPool<T>* tp = static_cast<threadPool<T>*>(args);
        string name = tp->getThreadName(pthread_self());

        // 检测任务队列中有没有任务? - 从线程池中获取任务
        while(true)
        {
            tp->Lock();
            while(tp->isQueueEmpty())
            {
                tp->threadSleep();
            }
            T task = tp->Pop();
            tp->unLock();
            // 执行任务...
            cout << "Task excute done : " << task << " " << name << endl;
            sleep(5);
        }
    }

    // 启动线程池
    void Start()
    {
        cout << "Start all thread ..." << endl;
        int num = _threads.size();
        for(int i = 0; i < num; i++)
        {
            _threads[i]._threadName = "Thread-" + to_string(i + 1);
            pthread_create(&_threads[i]._tid, nullptr, handleTask, this);
        }
    }

    // 将任务队列中的任务弹出来
    T Pop()
    {
        T t = _tasks.front();
        _tasks.pop();
        return t;
    }

    // 向任务队列中添加任务
    void Push(const T& t)
    {
        Lock();
        _tasks.push(t);
        wakeUp();
        unLock();
    }



    ~threadPool()
    { 
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_cond);
    }
    
private:
    vector<threadInfo> _threads;
    queue<T> _tasks;

    pthread_mutex_t _mutex;
    pthread_cond_t _cond;
};
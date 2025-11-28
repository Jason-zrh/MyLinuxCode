// 单例模式线程池
#pragma once
#include <iostream>
#include <queue>
#include <string>
#include <vector>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include "Task.hpp"


using namespace std;

struct threadInfo
{
    pthread_t _tid;
    string _threadName;
};

const int defaultNum = 5;

template<class T>
class threadPool
{
public:
// =================== 加锁解锁接口封装 ======================
    void Lock()
    {
        pthread_mutex_lock(&_mutex);
    }

    void unLock()
    {
        pthread_mutex_unlock(&_mutex);
    }

// ================== 休眠唤醒线程接口封装 =====================
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


public:
    // 类内非静态成员的参数列表里还有一个隐藏的threadPool this 这个参数，只有变成静态的函数才可以
    static void* handleTask(void* args)
    {
        threadPool<T>* tp = static_cast<threadPool<T>*>(args);

        // 检测任务队列中有没有任务? - 从线程池中获取任务
        while(true)
        {
            tp->Lock();
            // 防止假唤醒，用while循环保证任务队列中有任务
            while(tp->isQueueEmpty())
            {
                tp->threadSleep();
            }
            T task = tp->Pop();
            tp->unLock();
            // 执行任务...
            task();
        }
    }

    // 启动线程池
    void Start()
    {
        cout << "Start all thread ..." << endl;
        int num = _threads.size();
        for(int i = 0; i < num; i++)
        {
            _threads[i]._threadName = "Thread - " + to_string(i + 1);
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

 
    static threadPool<T>* GetInstance()
    {
        // 保证线程安全的同时，双重判断可以防止第二次访问对象的时候重复竞争锁
        if(_tp == nullptr)
        {
            pthread_mutex_lock(&_lock);
            if(_tp == nullptr)
            {
                _tp = new threadPool<T>();
            }
            pthread_mutex_unlock(&_lock);
        }
        return _tp;
    }

private:
    threadPool(int num = defaultNum) // 线程池内线程的数量
    :_threads(num)
    { 
        // 把锁和条件变量初始化
        pthread_mutex_init(&_mutex, nullptr);
        pthread_cond_init(&_cond, nullptr);
    }
    ~threadPool()
    { 
        // 销毁锁和条件变量
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_cond);
    }
    threadPool(const threadPool<T>& ) = delete;
    const threadPool<T>& operator=(const threadPool<T>&) = delete;

private:
    vector<threadInfo> _threads; // 储存每个线程的信息
    queue<T> _tasks;             // 任务队列
    pthread_mutex_t _mutex;      // 条件变量
    pthread_cond_t _cond;        // 锁


    // 单例模式:类内定义静态对象，类外初始化，将构造析构拷贝全部都私有
    static threadPool<T>* _tp;
    static pthread_mutex_t _lock;
};

// 类内定义类外初始化
template <class T>
threadPool<T>* threadPool<T>::_tp = nullptr;

template <class T>
pthread_mutex_t threadPool<T>::_lock = PTHREAD_MUTEX_INITIALIZER;
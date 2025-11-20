#pragma once
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <vector>
#include <semaphore.h>
#include <time.h>

using namespace std;

const static int defaultcap = 5;

template<class T>
class RingQueue
{
private:
    //	P（proberen）：测试、等待
    void P(sem_t& sem)
    {
        sem_wait(&sem);
    }

    // V（verhogen）：增加、释放 
    void V(sem_t& sem)
    {
        sem_post(&sem);
    }

    void Lock(pthread_mutex_t& mutex)
    {
        pthread_mutex_lock(&mutex);
    }

    void Unlock(pthread_mutex_t& mutex)
    {
        pthread_mutex_unlock(&mutex);
    }

public:
    RingQueue(int cap = defaultcap)
    :_ringQueue(cap)
    ,_cap(cap)
    ,_cStep(0)
    ,_pStep(0)
    {
        sem_init(&_cData, 0, 0);
        sem_init(&_pSpace, 0, cap);

        pthread_mutex_init(&_cMutex, nullptr);
        pthread_mutex_init(&_pMutex, nullptr);
    }

    void Push(const T& cin) // 生产者
    {
        P(_pSpace);
        Lock(_pMutex);
        _ringQueue[_pStep] = cin; // 生产成功
        _pStep = (_pStep + 1) % _cap;
        Unlock(_pMutex);
        V(_cData);
    }

    void Pop(T* out) // 消费者， 输出型参数
    {
        P(_cData);
        Lock(_cMutex);
        *out = _ringQueue[_cStep];
        _cStep = (_cStep + 1) % _cap;
        Unlock(_cMutex);
        V(_pSpace);
    }

    ~RingQueue()
    {
        sem_destroy(&_cData);
        sem_destroy(&_pSpace);

        pthread_mutex_destroy(&_cMutex);
        pthread_mutex_destroy(&_pMutex);

    }

private:
    vector<T> _ringQueue;
    int _cap;
    
    int _cStep;     // 消费者下标
    int _pStep;     // 生产者下标

    sem_t _cData;   // 消费者关注资源
    sem_t _pSpace;  // 生产者关注空间

    pthread_mutex_t _cMutex; // 消费者的锁
    pthread_mutex_t _pMutex; // 生产者的锁

};
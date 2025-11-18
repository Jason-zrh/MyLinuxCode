#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <queue>

using namespace std;


template <class T>
class blockQueue
{
public:
    blockQueue(int maxcap)
    :_maxcap(maxcap)
    { 
        pthread_mutex_init(&_mutex, nullptr);
        pthread_cond_init(&_c_cond, nullptr);
        pthread_cond_init(&_p_cond, nullptr);
    }


    // 消费者方法
    T Pop()
    {
        pthread_mutex_lock(&_mutex);
        if(_q.size() == 0)
        {
            pthread_cond_wait(&_c_cond, &_mutex);
        }
        T out = _q.front();
        _q.pop();
        pthread_cond_signal(&_p_cond);
        pthread_mutex_unlock(&_mutex);
        return out; 
    }


    // 生产者方法
    void Push(const T& cin)
    {
        // 为保证线程安全必须要加锁
        pthread_mutex_lock(&_mutex);
        // 同时这里是阻塞队列，不可能想生产的时候直接生产，需要确保生产条件满足
        if(_q.size() == _maxcap)
        {
            // 在调用的时候自动释放锁
            pthread_cond_wait(&_p_cond, &_mutex);
        }
        // 1.队列不满，2.线程被唤醒
        _q.push(cin);
        pthread_cond_signal(&_c_cond);
        pthread_mutex_unlock(&_mutex);
    }

    ~blockQueue()
    { 
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_p_cond);
        pthread_cond_destroy(&_c_cond);
    }
private:
    queue<T> _q;
    pthread_mutex_t _mutex; // STL容器本身不是线程安全的
    pthread_cond_t _c_cond;   // cp问题中有同步要求
    pthread_cond_t _p_cond;
    int _maxcap;
};
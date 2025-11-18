#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <vector>
using namespace std;

int cnt = 0;

// 将锁和条件变量都定义为全局的
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void* Count(void* args)
{
    // 将线程分离，当进程代码结束后会自动回收线程
    pthread_detach(pthread_self());
    uint64_t number = (uint64_t)args;
    cout << "thread: " << number << " create success" << endl;
    
    while(true)
    {
        // 加锁
        pthread_mutex_lock(&mutex);
        // 进行条件等待，在此处会自动加锁解锁
        pthread_cond_wait(&cond, &mutex);
        cout << "thread: " << number << " , count: " << cnt++ << endl;
        pthread_mutex_unlock(&mutex);
    }

    return nullptr;
}


int main()
{   
    // vector<pthread_t> tids;
    for(int i = 0; i < 5; i++)
    {
        pthread_t tid;
        pthread_create(&tid, nullptr, Count, (void*)i);
    }
    
    // 确保线程全部创建完成
    sleep(3);

    cout << "main thread ctrl begin ..." << endl;

    while(true)
    {
        // cout << "main thread ctrl begin ..." << endl;

        // 唤起该条件变量下的等待队列一次
        pthread_cond_signal(&cond);
        // 唤醒全部
        // pthread_cond_broadcast(&cond);
        sleep(1);
    }

    return 0;
}
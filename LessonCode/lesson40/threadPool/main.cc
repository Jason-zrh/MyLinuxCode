#include "threadPool.hpp"


int main()
{
    srand(time(nullptr) ^ getpid());
    // 创建一个线程池
    threadPool<int>* Pool = new threadPool<int>(5);
    Pool->Start();

    while(true)
    {
        Pool->Push(rand() % 10 + 1);
        usleep(1000);
    }

    return 0;
}
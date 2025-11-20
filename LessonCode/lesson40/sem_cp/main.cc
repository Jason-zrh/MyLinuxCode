#include "RingQueue.hpp"

void* Consume(void* args)
{
    // sleep(3);

    RingQueue<int>* rq = static_cast<RingQueue<int>*>(args);
    while(true)
    {
        // 消费数据
        int data = 0;
        rq->Pop(&data);

        // 处理数据
        cout << "Get a Data: "<< data << endl;
        sleep(1);
    }

    return nullptr;
}

void* Product(void* args)
{
    RingQueue<int>* rq = static_cast<RingQueue<int>*>(args);
    while(true)
    {
        // 获取数据
        int data = rand() % 10 + 1;

        // 生产数据
        rq->Push(data);
        cout << "Product done. Data: "<< data << endl;  
        // sleep(1);
    }

    return nullptr;
}

int main()
{
    srand(time(nullptr) ^ getpid());

    RingQueue<int>* rq = new RingQueue<int>();
    pthread_t c, p;
    pthread_create(&c, nullptr, Consume, rq);
    pthread_create(&p, nullptr, Product, rq);


    pthread_join(c, nullptr);
    pthread_join(p, nullptr);
    return 0;
}
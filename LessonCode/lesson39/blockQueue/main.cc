#include "blockqueue.hpp"

void* product(void* args)
{
    blockQueue<int>* bq = static_cast< blockQueue<int>*>(args);
    int data = 0;

    while(true)
    {
        // 生产
        data++;
        bq->Push(data);
        cout << "生产了一个数据: " << data << endl;
    }
    return nullptr;
}

void* consume(void* args)
{
    blockQueue<int>* bq = static_cast< blockQueue<int>*>(args);
    while(true)
    {   
        // 消费
        int data = bq->Pop();
        cout << "消费了一个数据: " << data << endl;
        sleep(2);
    }

    return nullptr;
}

int main()
{
    blockQueue<int>* bq = new blockQueue<int>(5);
    pthread_t productor, consumor;
    pthread_create(&productor, nullptr, product, bq);
    pthread_create(&consumor, nullptr, consume, bq);


    pthread_join(productor, nullptr);
    pthread_join(consumor, nullptr);
    delete bq;
    return 0;
}
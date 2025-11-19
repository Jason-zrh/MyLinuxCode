#include "blockqueue.hpp"
#include "Task.hpp"

void* product(void* args)
{
    blockQueue<Task>* bq = static_cast< blockQueue<Task>*>(args);
    // int data = 0;
    int x = 520, y = 1314;
    while(true)
    {
        // 生产
        // data++;
        // bq->Push(data);
        // cout << "生产了一个数据: " << data << endl;
        Task t(x, y);
        bq->Push(t);
        // 加载任务
        cout << "装填一个任务" << endl;
        sleep(2);
    }
    return nullptr;
}

void* consume(void* args)
{
    blockQueue<Task>* bq = static_cast< blockQueue<Task>*>(args);
    while(true)
    {   
        // 消费
        // int data = bq->Pop();
        // cout << "消费了一个数据: " << data << endl;
        // sleep(2);
        Task t = bq->Pop();
        int ret = t.run();
        cout << "计算一个任务: " << ret << endl;
    }

    return nullptr;
}

int main()
{
    blockQueue<Task>* bq = new blockQueue<Task>(5);
    pthread_t productor, consumor;
    pthread_create(&productor, nullptr, product, bq);
    pthread_create(&consumor, nullptr, consume, bq);


    pthread_join(productor, nullptr);
    pthread_join(consumor, nullptr);
    delete bq;
    return 0;
}
#include <iostream>
#include <unistd.h>
#include <pthread.h>

using namespace std;

void* threadRun(void* args)
{
    while(1)
    {
        cout << "new thread: " << getpid() << endl;
        sleep(1);
    }
    return nullptr;
}

int main()
{
    pthread_t tid;
    pthread_create(&tid, nullptr, threadRun, nullptr);

    while(1)
    {
        cout << "main thread: " << getpid() << endl;
        sleep(1);
    }
    return 0;
}
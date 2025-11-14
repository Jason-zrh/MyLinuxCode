#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <vector>

using namespace std;

#define NUM 10

class threadData
{
public:
    threadData(string threadname, string tid)
    :_threadname(threadname)
    ,_threadid(tid)
    { }
public:
    string _threadname;
    string _threadid;
};



void* threadRoutine(void* args)
{
    threadData* td = static_cast<threadData*>(args);
    int i = 10;
    while(i--)
    {
        cout << "My pid: " << getpid() << " I'm: " << td->_threadname << " tid: " << td->_threadid << endl;
        sleep(1);
    }
    delete td;
    return nullptr;
}

int main()
{
    vector<pthread_t> tids;
    for(int i = 0; i < NUM; i++)
    {
        pthread_t tid;
        char buffer[128];
        snprintf(buffer, sizeof(buffer), "0x%x", tid);
        threadData* td = new threadData(("Thread-" + to_string(i)), buffer);
        pthread_create(&tid, nullptr, threadRoutine, td);
        tids.push_back(tid);
        sleep(1);
    }


    for(int i = 0; i < tids.size(); i++)
    {
        pthread_join(tids[i], nullptr);
    }
    return 0;
}
#include <iostream>
#include <thread>
#include <chrono>
using namespace std;
using namespace std::chrono;
using namespace std::this_thread;
int main()
{
    while(1)
    {
        cout << "我是一个进程" << endl;
        sleep_for(seconds(2));
    }

    return 0;
}

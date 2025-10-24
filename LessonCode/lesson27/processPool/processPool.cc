#include "Task.hpp"

using namespace std;

int main()
{
    srand(time(nullptr) ^ getpid() ^ 1023);
    // 装载任务
    LoadTasks();
    // 先描述后组织，在父进程中用vector维护
    vector<channel> channels;

    // 1. 初始化 输出型参数
    InitProcess(&channels);

    // 2. 控制子进程    
    controlworker(channels);
    
    // 3. 清理收尾
    QuitProcess(channels);
    return 0;
}
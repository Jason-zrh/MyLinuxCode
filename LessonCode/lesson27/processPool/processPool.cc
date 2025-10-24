#include "Task.hpp"

using namespace std;

int main()
{
    // 先描述后组织，在父进程中用vector维护
    vector<channel> channels;
    // 1. 初始化
    InitProcess(&channels);

    Debug(channels);

    // 2. 控制子进程
    worker();

    // 3. 清理收尾
    return 0;
}
#include "common.hpp"

using namespace std;


int main()
{
    Init init; // 自动调用构造和析构来创建和销毁管道文件

    // 申请共享内存空间
    int shmid = SharedMemCreat();

    // 挂接共享内存空间
    char* shmaddr = (char*)shmat(shmid, nullptr, 0);
    lg(Info, "attach addr successfully, shmaddr: 0x%x", shmaddr);

    // 可以用命名管道来控制同步
    int fd = open(FILE_FIFO, O_RDONLY);
    if(fd < 0)
    {
        lg(Fatal, "open pipe file failed, error: %s", strerror(errno));
    }

    // ipc code
    while(true)
    {
        // 在接收信息前一定要通过管道收到通知
        char buffer[1];
        ssize_t n = read(fd, buffer, sizeof(buffer));
        if(n == 0) break;
        else if(n < 0) break;
        // 接收信息
        cout << "client say# " << shmaddr;
    }


    close(fd);

    // 主动取消挂接
    int n = shmdt(shmaddr);
    if(n < 0)
    {
        lg(Fatal, "shmdt failed, error: %s", strerror(errno));
    }
    lg(Info, "shmdt excute successfully!");

    // 删除共享内存
    int m = shmctl(shmid, IPC_RMID, nullptr);
    if(m < 0)
    {
        lg(Fatal, "shmctl failed, error: %s", strerror(errno));
    }

    return 0;
}
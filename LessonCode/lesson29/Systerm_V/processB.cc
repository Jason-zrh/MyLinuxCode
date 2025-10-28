#include "common.hpp"

using namespace std;



int main()
{
    // 获取共享内存码
    int shmid = SharedMemGet();
    // 挂接共享内存地址
    char* shmaddr = (char*)shmat(shmid, nullptr, 0);

    int fd = open(FILE_FIFO, O_WRONLY);
    if(fd < 0)
    {
        lg(Fatal, "open pipe file failed, error: %s", strerror(errno));
    }

    // ipc code
    while(true)
    {
        // 发信息
        cout << "Please Enter@ ";
        fgets(shmaddr, 4096, stdin);
        // 向管道中写一个字节
        write(fd, "z", 1);
    }


    close (fd);

    // 去挂接
    int n = shmdt(shmaddr);
    if(n < 0)
    {
        lg(Fatal, "shmdt failed, error: %s", strerror(errno));
    }
    lg(Info, "shmdt excute successfully!");

    return 0;
}
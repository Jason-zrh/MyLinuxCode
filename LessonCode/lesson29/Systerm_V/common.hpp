#ifndef __COMMON_HPP__ // if not define
#define __COMMON_HPP__


#include "log.hpp"
#include <iostream>
#include <cstring>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

using namespace std;


#define MEMSIZE 4096
#define FILE_FIFO "./myfifo"

// 引入日志系统
log lg;

enum
{
    FILE_CREAT_ERR = 1,
    FILE_UNLINK_ERR = 2,
    OPEN_ERROR = 3
};

class Init
{
public:
    Init()
    {
        // 创建信道
        int n = mkfifo(FILE_FIFO, 0664);
        if(n == -1)
        {
            perror("mkfifo");
            exit(FILE_CREAT_ERR);
        }
    }

    ~Init()
    {
        int m = unlink(FILE_FIFO);
        if(m == -1)
        {
            perror("unlink");
            exit(FILE_UNLINK_ERR);
        }       
    }
};


const string pathname  = "/home/mylinux/MyLinuxCode/LessonCode/lesson29/Systerm_V/keyfile.txt";
const int proj_id = 0x0318;

key_t Getkey()
{
    key_t key = ftok(pathname.c_str(), proj_id);
    if(key < 0)
    {
        lg(Fatal, "Get key failed, error: %s", strerror(errno));
        exit(1);
    }
    lg(Info, "Get key successfully, key: 0x%x", key);
    return key;
}


int SharedMemHelper(int flag)
{
    int shmid = shmget(Getkey(), MEMSIZE, flag);
    if(shmid == -1)
    {
        lg(Fatal, "Creat shared memory failed, error: %s", strerror(errno));
        exit(2);
    }
    lg(Info, "Creat shared memory successfully, shmid: %d", shmid);
    return shmid;
}

// 创建一个新的共享内存
int SharedMemCreat()
{
    return SharedMemHelper(IPC_CREAT | IPC_EXCL | 0666);
}

// 获取已有的共享内存
int SharedMemGet()
{
    return SharedMemHelper(IPC_CREAT);
}


#endif
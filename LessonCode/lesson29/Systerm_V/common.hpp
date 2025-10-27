#ifndef __COMMON_HPP__ // if not define
#define __COMMON_HPP__


#include "log.hpp"
#include <iostream>
#include <cstring>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <errno.h>
using namespace std;


#define MEMSIZE 4096

const string pathname  = "/home/mylinux/MyLinuxCode/LessonCode/lesson29/Systerm_V/keyfile.txt";
const int proj_id = 0x0318;

// 引入日志系统
log lg;

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

// 创建一个新的
int SharedMemCreat()
{
    return SharedMemHelper(IPC_CREAT | IPC_EXCL | 0666);
}

// 获取已有的
int SharedMemGet()
{
    return SharedMemHelper(IPC_CREAT);
}


#endif
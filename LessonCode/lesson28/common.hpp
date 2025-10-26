#pragma once
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cerrno>
#include <string>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <string.h>

#define FILE_FIFO "./myfifo"

enum{
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

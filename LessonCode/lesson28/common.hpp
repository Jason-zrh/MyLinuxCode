#pragma once
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cerrno>
#include <string>
#include <stdio.h>
#include <iostream>
#include <unistd.h>

#define FILE_FIFO "./myfifo"

enum{
    FILE_CREAT_ERR = 1,
    FILE_UNLINK_ERR = 2,
    OPEN_ERROR = 3
};

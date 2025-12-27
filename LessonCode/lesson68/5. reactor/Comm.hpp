#pragma once
#include <fcntl.h>

// 将文件描述符设置为非阻塞(ET模式必须),  每个文件描述符都要设置
void SetNoBlockOrDie(int sock)
{
    int fl = fcntl(sock, F_GETFL);
    if (fl > 0)
    {
        fcntl(sock, F_SETFL, fl | O_NONBLOCK);
    }
}
#pragma once
#include <fcntl.h>

// 将文件描述符设置为非阻塞(ET模式必须)
void SetNoBlockOrDie(int sock)
{
    int fl = fcntl(sock, F_GETFL);
    if (fl > 0)
    {
        fcntl(sock, F_SETFL, fl | O_NONBLOCK);
    }
}
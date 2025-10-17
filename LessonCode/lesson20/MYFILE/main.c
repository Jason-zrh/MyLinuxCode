#include "Myfile.h"

#define FILENAME "log.txt"



// 自己实现一个文件系统
int main()
{
    // 打开文件
    _FILE* fp = _fopen(FILENAME, "a");
    if(fp == NULL)
    {
        perror("_fopen");
        return 1;
    }

    // 向文件中写点东西
    const char* msg = "Hello Linux\n";

    int cnt = 5;
    while(cnt)
    {
        _fwrite(fp, msg, strlen(msg));
        sleep(1);
        cnt--;
    }

    // 关闭文件
    _fclose(fp);
    return 0;
}

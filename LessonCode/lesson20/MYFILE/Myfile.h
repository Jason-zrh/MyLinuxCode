/* 
#ifndef xxx
#define xxx

...

#endif 

上述操作等于 #prgram once 可以防止头文件被多次包含 
*/ 


#ifndef __MYFILE_H__
#define __MYFILE_H__ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


// 设定缓冲区大小
#define SIZE 1024
// 设定缓冲方法
#define FLUSh_NOW    1
#define FLUSH_LINE   2
#define FLUSH_ALL    4  


// 文件结构体
typedef struct IO_FILE
{
    // 文件描述符
    int _fileno;
    
    // 定义缓冲方法
    int buffermode;

    // 输入缓冲区
    char inbuffer[SIZE];
    int in_pos;

    // 输出缓冲区
    char outbuffer[SIZE];
    // 定位缓冲区文本位置
    int out_pos;
}_FILE;


// 打开文件
_FILE* _fopen(const char* filename, const char* flag);

// 写文件
int _fwrite(_FILE* fp, const char* msg, int len);

// 关闭文件
void _fclose(_FILE* fp);

// 冲刷缓冲区
void _fflush(_FILE* fp);

#endif 


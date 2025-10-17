#include "Myfile.h"

// 打开文件 文件名称，打开方法"w", "a", "r"
_FILE* _fopen(const char* filename, const char* flag)
{
    // 默认文件描述符为-1
    int fd = -1;
    // 覆盖的打开方式
    if(strcmp(flag, "w") == 0)
    {
        fd = open(filename, O_CREAT|O_WRONLY|O_TRUNC, 0666);
    }
    // 追加的打开方式
    else if(strcmp(flag, "a") == 0)
    {
        fd = open(filename, O_CREAT|O_WRONLY|O_APPEND, 0666);
    }
    // 只读打开方式
    else if(strcmp(flag, "r") == 0)
    {
        fd = open(filename, O_RDONLY);
    }
    // 错误的打开方式
    else 
    {
        perror("_fopen");
        return NULL;
    }
    
    if(fd == -1)
    {
        return NULL;
    }


    _FILE* fp = (_FILE*)malloc(sizeof(_FILE));
    if(fp == NULL)
    {
        perror("Malloc failed");
        return NULL;
    }
    


    // 默认为行缓冲模式
    fp->buffermode = FLUSH_ALL;
    // 缓冲区起始位置
    fp->out_pos = 0;
    fp->in_pos = 0;
    // 文件描述符
    fp->_fileno = fd;
    return fp;
}


// 向文件写点东西
int _fwrite(_FILE* fp, const char* msg, int len)
{
    // 先直接把消息写入缓冲区
    memcpy(&fp->outbuffer[fp->out_pos], msg, len);
    fp->out_pos += len; 
    // 确认缓冲模式
    // 立即缓冲模式
    if(fp->buffermode == FLUSh_NOW)
    {
        // 直接把缓冲区的消息写入内核
        write(fp->_fileno, fp->outbuffer, fp->out_pos);
        fp->out_pos = 0;
    }
    // 行缓冲模式
    else if(fp->buffermode == FLUSH_LINE)
    {
        if(fp->outbuffer[fp->out_pos - 1] == '\n')
        {
            write(fp->_fileno, fp->outbuffer, fp->out_pos);
            fp->out_pos = 0;
        }
    }
    // 全缓冲模式
    else if (fp->buffermode == FLUSH_ALL)
    {
        if(fp->out_pos == SIZE)
        {
            write(fp->_fileno, fp->outbuffer, fp->out_pos);
            fp->out_pos = 0;
        }
    }
    else 
    {
        // 发生错误
        return 0;
    }
    return len;
}

// 刷新缓冲区
void _fflush(_FILE* fp)
{
    if(fp->out_pos > 0)
    {
        write(fp->_fileno, fp->outbuffer, fp->out_pos);
        fp->out_pos = 0;
    }
}

// 关闭文件
void _fclose(_FILE* fp)
{
    // 在关闭前冲刷缓冲区
    _fflush(fp);
    close(fp->_fileno);
    free(fp);
}


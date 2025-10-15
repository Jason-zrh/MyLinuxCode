#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

// 系统调用open，write，close
int main()
{
    // 返回值代表文件描述符，打开文件的方法是读写，追加的形式
    int fd = open("log.txt", O_RDWR|O_APPEND);
    const char* msg = "Hello Linux\n";
    write(fd, msg, strlen(msg));
    close(fd);
    return 0;
}




// 测试下标0，1，2
//int main()
//{
//    // 0 -- stdin
//    //char buffer[1024];
//    //ssize_t n = read(0, buffer, sizeof(buffer) - 1);
//    //// 读入以后把最后一位放上\0
//    //buffer[n] = '\0';
//    //printf("stdin: %s", buffer);
//
//    // 1 -- stdout
//    const char* msg = "Hello Linux\n";
//    write(1, msg, strlen(msg));
//    return 0;
//}


//int main()
//{
//    // 设置掩码
//    umask(0);
//    // 创建文件并设置权限
//    
//    // fd: file descriptor 文件描述符, fd : int
//    // O_RDONLY: 只读打开
//    // O_WRONLY: 只写打开
//    // O_RDWR : 读，写打开
//    // 上述三个打开方式必须有一个且只能有一个
//    // O_CREAT : 若文件不存在，则创建它。需要使用mode选项，来指明新文件的访问权限
//    // O_APPEND: 追加写
//    // O_TRUNC 清空后写入
//    
//    int fd = open("log.txt", O_WRONLY|O_CREAT, 0666);
//    if(fd == -1)
//    {
//        printf("open file failed\n");
//        return 1;
//    }
//
//    close(fd);
//    return 0;
//}



// 比特位方式的标志位传递方式
//#define ONE (1 << 0)      // 1
//#define TWO (1 << 1)      // 2 
//#define THREE (1 << 2)    // 4
//#define FOUR (1 << 3)     // 8
//
//void show(int flags)
//{
//    if(flags & ONE)
//    {
//        printf("Func one\n");
//    }
//    
//    if(flags & TWO)
//    {
//        printf("Func two\n");
//    }
//    if(flags & THREE)
//    {
//        printf("Func three\n");
//    }
//    if(flags & FOUR)
//    {
//        printf("Func four\n");
//    }
//}
//
//int main()
//{
//    printf("-------------------\n");
//    show(ONE);
//    printf("-------------------\n");
//    show(ONE | TWO);
//    printf("-------------------\n");
//    show(TWO | THREE);
//    printf("-------------------\n");
//    show(ONE | TWO | THREE);
//    printf("-------------------\n");
//    
//    return 0;
//}









//int main()
//{
//    // 可以先更改工作目录, 更改工作目录后，将会在目标路径直接创建文件
//    //chdir("/home/MyLinux");
//    
//    // printf("pid: %d\n", getpid());
//    // 默认在当前路径下创建， 即进程当前路径 -> cwd
//    FILE* fp = fopen("log.txt", "w");
//    if(fp == NULL)
//    {
//        perror("fopen");
//        return 1;
//    }
//
//    // 写入文件
//    const char* message = "Hello Linux Message!";
//    fwrite(message, strlen(message), 1, fp);
//
//
//    fclose(fp);
//    return 0;
//}

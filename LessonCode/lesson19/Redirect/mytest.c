#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>


#define filename "log.txt"

// 输入重定向
int main()
{
    int fd = open(filename, O_RDONLY);
    if(fd < 0)
    {
        perror("open");
        return 1;
    }

    dup2(fd, 0);
    char buffer[1024];
    ssize_t s = read(0, buffer, sizeof(buffer) - 1);
    if(s > 0)
    {
        buffer[s] = '\0';
        // 读取到东西了进行输出
        printf("echo# %s\n", buffer);
    }
    

    close(fd);
    return 0;
}



// 输出重定向
// 重定向系统调用接口duplacation - dup2()
// int main()
// {
//     int fd = open(filename, O_WRONLY|O_CREAT|O_APPEND, 0666);
//     if(fd < 0)
//     {
//         perror("open");
//         return 1;
//     }
//     // 重定向 - 两个参数 oldfd, newfd 接口的作用是把旧的fd复制到新的fd上，使两个文件描述符的值都是fd
//     
//     dup2(fd, 1);
// 
//     const char* msg = "Hello Linux\n";
//     int cnt = 5;
//     while(cnt)
//     {
//         write(1, msg, strlen(msg));
//         cnt--;
//     }
//     return 0;
// }


// 下面就是一个重定向原理，将本来应该输入给显示器的文本重定向给我们的log.txt --- 进程只关注文件描述符！！！
// int main()
// {
// 
//     // close(1);
//     // 将0，1，2三个标准流关掉后，打开文件的下标将会从低到高遍历找到最小的下标返回
//     int fd = open(filename, O_WRONLY|O_CREAT|O_TRUNC, 0666);
//     if(fd < 0)
//     {
//         // fd - 文件描述符是文件数组的下标，所以不会出现负数
//         perror("open");
//         return 1;
//     }
//     // 打印文件描述符
//     // printf("fd: %d\n", fd);
//    
//     const char* msg = "Hello Linux\n";
//     int cnt = 5;
//     while(cnt)
//     {
//         write(1, msg, strlen(msg));
//         cnt--;
//     }
// 
//     close(fd);
//     return 0;
// }




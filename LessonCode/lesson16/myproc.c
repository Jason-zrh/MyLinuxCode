#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

void show()
{
    printf("Hello, Linux\n");
    printf("Hello, Linux\n");
    printf("Hello, Linux\n");
    printf("Hello, Linux\n");
    printf("Hello, Linux\n");
    printf("Hello, Linux\n");
    printf("Hello, Linux\n");
    exit(1);// exit只要一触发就是要使进程终止
    printf("Hello, Linux\n");
    printf("Hello, Linux\n");
    printf("Hello, Linux\n");
    printf("Hello, Linux\n");
    printf("Hello, Linux\n");
    printf("Hello, Linux\n");
}

int main()
{
    show();
    printf("Hello, main!\n");
    //printf("Hello, Linux\n");
    //exit(1); // 与return 1; 等价
    return 0;
}




// 测试进程异常退出
// 使用指令kill -l 可以看到所有进程异常终止的情况代码
//int main()
//{
//    char* p = NULL;
//    *p = "Hello world";
//
//
//    return 0;
//}


//int main()
//{
//    int ret = 0;
//    // 申请内存大概率失败
//    char* p = (char*)malloc(1000 * 1000 * 1000 * 4);
//    if(p == NULL)
//    {
//        printf("malloc failed: %d, %s\n", errno, strerror(errno));
//        ret = errno;
//    }
//    else 
//    {
//        // 内存使用逻辑
//        printf("malloc success");
//    }
//    return ret;
//}


// 可以通过错误列表自己定义错误类型
//const char* errString[] = {
//    "success",
//    "error 1",
//    "error 2",
//    "error 3",
//    "error 4",
//};


//int main()
//{
//    int i = 0;
//    for(i = 0; i < 200; i++)
//    {
//        printf("[%d]: %s\n",i, strerror(i));
//    }
//    return 0;
//}

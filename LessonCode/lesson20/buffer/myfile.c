#include <stdio.h>
#include <string.h>
#include <unistd.h>


int main()
{
    const char* fstr = "Hello fwrite\n";
    const char* str = "Hello write\n";

    printf("Hello printf\n");
    fprintf(stdout, "Hello fprintf\n");
    fwrite(fstr, strlen(fstr), 1, stdout);
    
    write(1, str, strlen(str));

    fork();
    return 0;
}

// int main()
// {
    // 缓冲区存在原因
    // 1.解决用户效率问题
    // 2.配合进行格式化输出
    


    // 如果在数据被从c缓冲区写到内核缓冲区前close(1)了，数据就会被丢失
    // printf("Hello Linux");
    // close(1);


    // printf("Hello ");
    // printf("Linux\n");
    
    //  --------------
    // fwrite / fprintf 
    //  --------------
    //     C缓冲区
    //  --------------
    // 系统调用write()
    //  --------------
    //   内核缓冲区
    //  --------------


    // return 0;
// }

// int main()
// {
//     const char* fstr = "Hello fwrite\n";
//     const char* str = "Hello write\n";
// 
//     // C式接口 -- 如果把\n去掉则不会打印，说明他们不在系统缓冲区中！！！
//     // 显示器文件的刷新方式是行刷新，所以在printf结束后遇到\n就会立即将数据刷新
//     // 用户刷新的本质就是: 1 + write写入内核中
//     printf("Hello printf\n");
//     fprintf(stdout, "Hello fprintf\n");
//     fwrite(fstr, strlen(fstr), 1, stdout);
//     
//     // 缓冲区刷新的三种策略
//     // 1.无缓冲 - 每次写入缓冲区后，直接执行fflush(stdout)，刷新缓冲区中的数据
//     // 2.行缓冲 - 当识别到\n时把缓冲区中的数据在一行输出
//     // 3.全缓冲 - 缓冲区满了才刷新
// 
//     // 系统调用接口
//     write(1, str, strlen(str));
// 
//     close(1);
//     return 0;
// }

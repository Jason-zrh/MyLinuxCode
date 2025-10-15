#include<stdio.h>
#include<stdlib.h>
#include<string.h>


// 我们可以用getenv()函数获得环境变量

//int main()
//{
//    char user[30];
//    strcpy(user, getenv("USER"));
//    if(strcmp(user, "root") == 0)
//    {
//        printf("超级用户，允许它做任何事\n");
//    }
//    else 
//    {
//        printf("普通用户，访权限受限制\n");
//    }
//    return 0;
//}



// 命令行参数
// int argc, char* argv 就是命令行参数，与选项有关
//int main(int argc, char* argv[])
//{
//    int i = 0;
//    for(i = 0; i < argc; i++)
//    {
//        printf("argv[%d] -> %s\n",i, argv[i]);
//    }
//    return 0;
//}
// 效果如下:
//[MyLinux@myserver lesson13]$ ./mycmd -a -b -c -d
//argv[0] -> ./mycmd
//argv[1] -> -a
//argv[2] -> -b
//argv[3] -> -c
//argv[4] -> -d

// 这里可以看出来命令行参数与指令的选项有关，如ls -a, ls -l

//int main(int argc, char* argv[])
//{
//    if(argc != 2)
//    {
//        printf("Usage: ./mycmd -[a|b|c|d]\n");
//    }
//    else if(strcmp(argv[1], "-a") == 0)
//    {
//        printf("Func1\n");
//    }
//    else if(strcmp(argv[1], "-b") == 0)
//    {
//        printf("Func2\n");
//    }
//    else if(strcmp(argv[1], "-c") == 0)
//    {
//        printf("Func3\n");
//    }
//    else if(strcmp(argv[1], "-d") == 0)
//    {
//        printf("Func4\n");
//    }
//    else 
//    {
//        printf("Default Func\n");
//    }
//
//    return 0;
//}

// 模拟实现命令行选项的支持（demo）

int main(int argc, char* argv[], char* env[])
{
    printf("MYVAL: %s\n",getenv("MYVAL"));
    return 0;
}

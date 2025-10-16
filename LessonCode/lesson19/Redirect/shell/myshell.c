#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <error.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

#define LINE_SIZE 1024
#define ARGC 30
// strtok自动截取字符串的分割符
#define DELIM " "
#define EXIT_CODE -1


// 重定向标识符   
#define IN_RDIR          0   
#define OUT_RDIR         1
#define APPEND_RDIR      2
#define NONE            -1


// 命令存储
char commandline[LINE_SIZE];
// 命令解析(arguement vector)
char* argv[ARGC];
// 存储上一个子进程的退出码
int lastcode = 0;
char pwd[LINE_SIZE];


// 重定向标记
char* redirfilename = NULL;
int redir = NONE;

const char* getusername()
{
    return getenv("USER");
}


const char* gethostname()
{
    return getenv("HOSTNAME");
}
    
void getpwd()
{
    getcwd(pwd, sizeof(pwd) - 1);
}


void check_redir(char* cmd)
{
    // 检查cmd中是否有 > />> /<标识
    char* pos = cmd;
    while(*pos)
    {
        if(*pos == '>')
        {
            // 可能有两种情况
            if(*(pos + 1) == '>')
            {
                redir =  APPEND_RDIR;
                // 追加重定向
                *pos++ = '\0';
                *pos++ = '\0';
                while(isspace(*pos))pos++;
                redirfilename = pos;
                break;
            }
            else 
            {
                // 输出重定向
                *pos = '\0';
                pos++;
                // 更改标识符
                redir = OUT_RDIR;
                // 找出重定向文件名字
                while(isspace(*pos))pos++;
                redirfilename = pos;
                break;
            }
        }
        else if(*pos == '<')
        {
            // 输入重定向
            // 先把<符号替换成\0
            *pos = '\0';
            pos++;
            // 更改标识符
            redir = IN_RDIR;
            // 找出重定向文件名字
            while(isspace(*pos))pos++;

            redirfilename = pos;
            break;
        }
        else 
        {
            // do nothing
        }

        pos++;
    }
}

void Interact(char* cline, size_t size)
{
    getpwd();
    printf("[%s@%s %s]$ ",getusername(), gethostname(), pwd);
    // 用fgets函数可以获取一行字符串，防止scanf碰到空格就停下，三个参数分别为存储输入的地址，输入的大小，和输入位置(一般为stdin)
    char* s = fgets(cline, size, stdin);
    (void)s;
    // 把字符串最后的\n去掉
    cline[strlen(cline) - 1] = '\0';

    // 在这里检查是否存在重定向标识符
    check_redir(cline);
}


size_t splitString(char* argv[], char commandline[])
{
    size_t i = 0; 
    argv[i++] = strtok(commandline, DELIM);
    while(argv[i++] = strtok(NULL, DELIM));
    return i - 1;
}


void NormalExecute()
{
    pid_t id = fork();
    if(id < 0)
    {
        // 发生错误
        perror("fork");
        return;
    }
    else if(id == 0)
    {
        // 子进程
        int fd = 0;
        // 这里检查标识符有没有被修改过
        if(redir == IN_RDIR)
        {
            // 打开文件
            fd = open(redirfilename, O_RDONLY);
            dup2(fd, 0);
        }
        else if(redir == OUT_RDIR)
        {
            fd = open(redirfilename, O_WRONLY|O_CREAT|O_APPEND, 0666);
            dup2(fd, 1);
        }
        else if(redir == APPEND_RDIR)
        {
            fd = open(redirfilename, O_WRONLY|O_CREAT|O_APPEND, 0666);
            dup2(fd, 1);
        }
        execvp(argv[0], argv);
        exit(EXIT_CODE);
    }
    else 
    {
        // 父进程进行等待
        int st = 0;
        pid_t rid = waitpid(id, &st, 0);
        if(rid == id)
        {
            // 更新子进程的退出码
            lastcode = WEXITSTATUS(st);
        }
    }
}


int main()
{
    
    size_t quit = 0;
    while(!quit)
    {
        // 在每次检查的最开始把标记符号重新置NONE
        redir = NONE;
        redirfilename = NULL;
        // 命令的输入
        Interact(commandline, LINE_SIZE);
        
        // 解析指令
        // argc(arguement count)
        size_t argc = splitString(argv, commandline);
        if(argc == 0)
        {
            continue;
        }

        // 判断是内建指令还是普通指令，如果是内建命令就直接让父进程执行，由此可知，内建命令可以视作shell的一个函数
        if(argc == 2 && strcmp(argv[0], "cd") == 0)
        {
            // 更改路径以后还要更改环境变量
            chdir(argv[1]);
            // sprintf可以进行替换
            getpwd();
            sprintf(getenv("PWD"), "%s", pwd);
            continue;
        }

        // 普通指令的执行
        NormalExecute();
    }
    return 0;
}

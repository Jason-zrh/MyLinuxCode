#include "common.hpp"
#include "log.hpp"

using namespace std;


// 管理管道文件
int main()
{
    // 自动调用构造和析构创建和删除管道文件
    Init init;
    log lg;
    lg.Enable(Classfile);

    // 打开信道
    int fd = open(FILE_FIFO, O_RDONLY);
    if(fd < 0)
    {   
        lg(Fatal, "error message: open pipe failed, errno: %d", errno);
        exit(OPEN_ERROR);
    }
    
    // cout << "server file open" << endl;
    lg(Info, "Server file open success");
    // 开始通信
    while(true)
    {
        char buffer[1024] = {0};
        int x = read(fd, buffer, sizeof(buffer));
        if(x > 0)
        {
            buffer[x] = 0;
            cout << "client say# " << buffer << endl;
        }
        else if(x == 0)
        {
            // cout << "client closed, me too" << endl;
            lg(Info, "client closed, me too!");
            break;
        }
        else
        {
            lg(Fatal, "communicate failed!");
            break;        
        }

    }

    // 关闭信道
    close(fd);
    lg(Info, "close pipe");
    return 0;
}
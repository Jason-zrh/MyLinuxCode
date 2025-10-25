#include "common.hpp"

using namespace std;


// 管理管道文件
int main()
{
    // 创建信道
    int n = mkfifo(FILE_FIFO, 0664);
    if(n == -1)
    {
        perror("mkfifo");
        exit(FILE_CREAT_ERR);
    }

    // 打开信道
    int fd = open(FILE_FIFO, O_RDONLY);
    if(fd < 0)
    {
        perror("open");
        exit(OPEN_ERROR);
    }
    
    cout << "server file open" << endl;
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
            cout << "client closed, me too" << endl;
            break;
        }
        else
            break;        
    }


    // 关闭信道
    close(fd);
    int m = unlink(FILE_FIFO);
    if(m == -1)
    {
        perror("unlink");
        exit(FILE_UNLINK_ERR);
    }
    return 0;
}
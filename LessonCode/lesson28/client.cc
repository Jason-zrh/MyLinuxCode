#include "common.hpp"
#include "log.hpp"

using namespace std;

int main()
{
    log lg;
    lg.Enable(Classfile);
    // 客户端以只写方式打开文件
    int fd = open(FILE_FIFO, O_WRONLY);
    if(fd < 0)
    {
        lg(Fatal, "error message: open pipe failed, errno: %d", errno);
        exit(OPEN_ERROR);
    }

    lg(Info, "open pipe success!");
    string line;
    while(true)
    {
        cout << "Please Enter@ ";
        getline(cin, line);
        if(strcmp(line.c_str(), "0") == 0)
            break;
        write(fd, line.c_str(), sizeof(line));
    }

    // 关闭文件
    close(fd);
    lg(Info, "close pipe");
    return 0;
}
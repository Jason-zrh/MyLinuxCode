#include "common.hpp"

using namespace std;

int main()
{
    // 客户端以只写方式打开文件
    int fd = open(FILE_FIFO, O_WRONLY);
    if(fd < 0)
    {
        perror("open");
        exit(OPEN_ERROR);
    }

    string line;
    while(true)
    {
        cout << "Please Enter@ ";
        getline(cin, line);
        write(fd, line.c_str(), sizeof(line));
    }

    // 关闭文件
    close(fd);
    return 0;
}
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <cstdio>
#include <cerrno>
#include <cstring>

using namespace std;

void SetNonBlock(int fd)
{
    // 先获取标志位
    int fl = fcntl(fd, F_GETFL);
    if(fl < 0)
    {
        perror("fcntl");
        return;
    }
    // 设置标志位
    fcntl(fd, F_SETFL, fl | O_NONBLOCK);
    cout << fd << "NON_BLOCK has been setted" << endl;
}


int main()
{
    SetNonBlock(0);

    char buffer[1024];
    while(1)
    {
        // printf("Please Enter: ");
        // fflush(stdout);
        ssize_t n = read(0, buffer, sizeof(buffer));
        if(n > 0)
        {
            buffer[n - 1] = 0;
            cout << "echo : " << buffer << endl;
        }
        else if(n == 0)
        {
            cout << "read done" << endl;
            break;
        }
        else
        {
            // 非阻塞时当资源未就绪报的是错误码
            if(errno == EWOULDBLOCK)
            {
                cout << "资源未就绪, 再给劳资等一会" << endl;
                sleep(1);
                continue;
            }
            else
            {
                cerr << "read error: " << strerror(errno) << endl;
                break;
            }
        }
    }
    return 0;
}
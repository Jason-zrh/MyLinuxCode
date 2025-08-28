#include <iostream>
#include <unistd.h>


using namespace std;

// 打印进度条函数
void printProgressBar(int progress, int total, int barWidth = 100)
{
    float fraction = (float)progress / total;
    int pos = barWidth * fraction;

    cout << "[";
    for (int i = 0; i < barWidth; ++i) 
    {
        if (i < pos) 
            cout << "=";
        else if 
            (i == pos) cout << ">";
        else cout << " ";
    }
    cout << "] " << int(fraction * 100.0) << " %\r";
    cout.flush();
}

int main() 
{
    const int total = 100;

    for (int i = 0; i <= total; ++i)
    {
        printProgressBar(i, total);
        usleep(50 * 1000);
    }

    cout << endl;
    return 0;
}



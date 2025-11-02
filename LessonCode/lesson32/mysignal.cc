#include <iostream>
#include <signal.h>
#include <unistd.h>

using namespace std;

void handler(int signo)
{
    cout << "catch signal: " << signo << endl;
    exit(1);
}

int main()
{
    signal(SIGFPE, handler);
    int a = 10;
    int b = 0;
    int c = a / b;
    cout << c << endl;
    return 0;
}
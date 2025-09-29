#include<iostream>
using namespace std;


int main(int argc, char* argv[], char* envp[])
{
    cout << "命令行参数:" << endl;
    for(int i = 0; i < argc; i++)
    {
        cout << i << ":" << argv[i] << endl;
    }


    cout << "环境变量:" << endl;
    for(int i = 0; envp[i] != nullptr; i++)
    {
        cout << i << ":" << envp[i] << endl;
    }

    return 0;
}

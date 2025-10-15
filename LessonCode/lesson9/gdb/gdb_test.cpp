#include<iostream>

using namespace std;

int addToTop(int top)
{
    int ret = 0;
    for(int i = 0; i <= top; i++)
    {
        ret += i;
    }
    return ret;
}


int main()
{
    cout << "debug begin" << endl;
    int top = 100;
    int sum = addToTop(top);

    cout << "The sum is: " << sum << endl;

    cout << "debug end" << endl;
    return 0;
}

#pragma once
#include <iostream>

using namespace std;

class Task
{
public:
    Task(int x, int y)
    :_x(x)
    ,_y(y)
    { }
    
    int run()
    {
        return _x + _y;
    }

    ~Task()
    { }

private:
    int _x;
    int _y;
};
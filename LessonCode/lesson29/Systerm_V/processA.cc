#include "common.hpp"

using namespace std;

int main()
{
    int shmid = SharedMemCreat();
    cout << shmid << endl;
    return 0;
}
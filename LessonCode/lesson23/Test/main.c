#include "mymath.h"

int main()
{
    int n = div(1, 0);
    // printf("1 + 1 = %d\n", add(1, 1));
    printf("1 / 0 = %d, errno =  %d\n", n, errorno);
    return 0;
}

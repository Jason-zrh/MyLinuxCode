#include <iostream>
#include <vector>
using namespace std;

// 前缀和

int main()
{
    int n, m;
    cin >> n >> m;
    vector<int> nums;
    nums.resize(n + 1);
    for(int i = 1; i < n + 1; i++)
    {
        cin >> nums[i];
    }

    vector<int> f;
    f.resize(n + 1);
    f[0] = 0;
    for(int i = 1; i < n + 1; i++)
    {
        f[i] = f[i - 1] + nums[i];
    }


    while (m)
    {
        int l, r;
        cin >> l >> r;
        cout << f[r] - f[l - 1] << endl;
        m--;
    }
    return 0;
}

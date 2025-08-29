#include<iostream>
#include<vector>


using namespace std;

int main()
{
    int n;
    cin >> n;
    vector<int> prefix;
    prefix.resize(n + 1);
    for(int i = 0; i < n; i++)
    {
        cin >> prefix[i];
    }

    for(int i = 0; i < n; i++)
    {
        cout << prefix[i] << " ";
    }
    cout << endl;
    return 0;
}

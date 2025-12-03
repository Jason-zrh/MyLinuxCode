#include <iostream>
#include <unistd.h>
#include <jsoncpp/json/json.h>

using namespace std;

// json实现序列化和反序列化
int main()
{
    // 1.创建json万能对象
    Json::Value root;
    // 2.[]中的对象名，后面的是对象的值
    root["x"] = 520;
    root["y"] = 1314;
    root["op"] = '+';

    // 3.序列化
    // Json::FastWriter w; // 快速序列化
    Json::StyledWriter w; // 可视化更好的序列化
    string res = w.write(root);

    // cout << res;

    // 4.反序列化
    Json::Value v; // 用来接收反序列化后的对象
    Json::Reader r; // 读者，用来反序列化
    r.parse(res, v); // 反序列化

    int x = v["x"].asInt();
    char op = v["op"].asInt();
    int y = v["y"].asInt();
    cout << "Receve a msg: "<< x << " " << op << " " << y << endl;

    return 0;
}
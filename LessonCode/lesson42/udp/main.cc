#include "UdpServer.hpp"
#include "log.hpp"
#include <stdio.h>
#include <vector>

using namespace std;

// 这里是处理方法的回调函数
string Handler(const string& str, const string& clientip, uint16_t clientport)
{
    cout << "[" << clientip << ":" << clientport << "]" << str << endl;
    string res = "Serevr get a msg: ";
    res += str;
    cout << res << endl;
    return res;
}

// 检查命令中有没有坏蛋命令
bool safeCheck(const string& cmd)
{
    vector<string> key_word{
        "rm",
        "mv",
        "cp",
        "yum",
        "unlink"
    };

    for(auto& word : key_word)
    {
        auto pos = cmd.find(word);
        if(pos != string::npos)
        {
            return false;
        }
    }
    return true;
}

// 自带执行命令的接口
string excuteCommand(const string& cmd)
{
    cout << "Get a cmd: " << cmd << endl;
    // 先进行命令是否合法的检查
    if(!safeCheck(cmd))
    {
        return "Bad man!";
    }
    FILE* fp = popen(cmd.c_str(), "r");
    if(fp == nullptr)
    {
        perror("popen");
        return "error";
    }
    string res;
    char buffer[4096];
    while(true)
    {
        char* got = fgets(buffer, sizeof(buffer), fp);
        if(got == nullptr)
            break;
        res += got;
    }
    pclose(fp);
    return res;
}


int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        Usage(argv[0]);
        exit(0);
    }

    uint16_t port = stoi(argv[1]);
    unique_ptr<UdpServer> svr(new UdpServer(port));

    svr->Init();
    svr->Run();
    
    return 0;
}
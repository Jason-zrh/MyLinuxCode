// ====================== 协议定制 ======================
#pragma once
#include <iostream>
using namespace std;


const string blank_sep_space = " ";
const string protocol_sep = "\n";

// ====================== 添加报头 ======================
string Encode(string& content) // "x op y" 
{
    string package = to_string(content.size());
    package += protocol_sep;
    package += content;
    package += protocol_sep;
    return package; // "len\nx op y\n"
}

// ====================== 删除报头 ======================
bool Decode(string& package, string* content) // "len\nx op y\n"
{
    size_t pos = package.find(protocol_sep);
    // 找不到分隔符
    if(pos == std::string::npos) 
        return false;
    // 找到描述长度的串
    string len_str = package.substr(0, pos);
    // 计算算式的长度
    size_t len = stoi(len_str);
    // 报头 + 算式 + 两\n
    size_t total_len = len_str.size() + len + 2;
    // 长度不符合协议要求
    if(package.size() < total_len) 
        return false;

    *content = package.substr(pos+1, len);
    // earse 移除报文 package.erase(0, total_len);
    // 报文已经被拿走了
    package.erase(0, total_len);
    return true;
}

// ====================== 请求载荷 ======================
class Request
{
public:
    Request(const int& data1, const int& data2, const char& op)
    :_x(data1)
    ,_y(data2)
    ,_op(op)
    { }

    Request()
    { }

    ~Request()
    { }
public:
// --------------------- 序列化 ---------------------
    bool Serialization(string* out) // "x op y"
    {
        // 构建报文的有效载荷
        string s = to_string(_x);
        s += blank_sep_space;
        s += _op;
        s += blank_sep_space;
        s += to_string(_y);
        *out = s;
        return true;
    }
// --------------------- 反序列化 ---------------------
    bool Deserialization(const string& in) // "x op y"
    {
        string part_x, part_y;
        size_t left = in.find(blank_sep_space);
        if(left == string::npos)
            return false;
        part_x = in.substr(0, left); // substr左闭右开
        size_t right = in.rfind(blank_sep_space);
        if(right == string::npos)
            return false;
        part_y = in.substr(right + 1);
        if(left + 2 != right)
            return false;
        _op = in[left + 1];
        _x = stoi(part_x);
        _y = stoi(part_y);
        return true;
    }
public:
    int _x;
    int _y;
    char _op; // + - * / %
};


// ====================== 回复载荷 ======================
class Response
{
public:
    Response(const int& res = 0, const int& code = 0)
    :_result(res)
    ,_code(code)
    { }

    ~Response()
    { }
// --------------------- 序列化 ---------------------
    bool Serialization(string* out) // "res code"
    {
        // 构建报文的有效载荷
        string s = to_string(_result);
        s += blank_sep_space;
        s += to_string(_code);
        *out = s;
        return true;
    }
// --------------------- 反序列化 ---------------------
    bool Deserialization(const string& in) // "res code"
    {
        size_t pos = in.find(blank_sep_space);
        if(pos == string::npos)
            return false;
        _result = stoi(in.substr(0,pos));
        _code = stoi(in.substr(pos + 1));
        return true;
    }

public:
    int _result;
    int _code;    // 计算结果可信度 0表示可信
};
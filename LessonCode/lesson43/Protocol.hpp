// ====================== 协议定制 ======================
#pragma once
#include <iostream>
using namespace std;


const string blank_sep_space = " ";
const string protocol_sep = "/n";

// ====================== 添加报头 ======================
const string& Encode(string& content)
{
    string package = to_string(content.size());
    package += protocol_sep;
    package += content;
    package += protocol_sep;
    return package;
}

// ====================== 删除报头 ======================
const string& Decode()
{

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

    ~Request()
    { }
public:
// --------------------- 序列化 ---------------------
    bool Serialization(string* out)
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
    bool Deserialization(const string& in)
    {

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
    Response(const int& res, const int& code)
    :_result(res)
    ,_code(code)
    { }

    ~Response()
    { }
// --------------------- 序列化 ---------------------
    bool Serialization(string* out)
    {
        // 构建报文的有效载荷
        string s = to_string(_result);
        s += blank_sep_space;
        s += to_string(_code);
        *out = s;
        return true;
    }
// --------------------- 反序列化 ---------------------
    bool Deserialization()
    {

    }
public:
    int _result;
    int _code;    // 计算结果可信度 0表示可信
};
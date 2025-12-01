#pragma once
#include <iostream>
#include "Protocol.hpp"
using namespace std;

enum{
    DIV_ZERO = 1,
    MOD_ZERO = 2,
    OTHRE_OP = 3,
};

class FuncServer
{
public:
    FuncServer()
    { }

    Response CaculatorHelper(const Request& req)
    {
        Response resp(0, 0);
        switch(req._op)
        {
            case '+':
                resp._result = req._x + req._y;
                break;
            case '-':
                resp._result = req._x - req._y;
                break;
            case '*':
                resp._result = req._x * req._y;
                break;
            case '/':
            {
                if(req._y == 0)
                {
                    resp._result = -1;
                    resp._code = DIV_ZERO; // 除数为0
                }
                else
                {
                    resp._result = req._x / req._y;
                }
            }
            break;
            case '%':
            {
                if(req._y == 0)
                {
                    resp._result = -1;
                    resp._code = MOD_ZERO; // 除数为0
                }
                else
                {
                    resp._result = req._x % req._y;
                }
            }
            break;
            default:
                resp._code = OTHRE_OP;
            break;
        }
        return resp;
    }

    string Caculator(string& package)
    {
        // 解析报头
        string content;
        bool r = Decode(package, &content);
        if(!r) return ""; // 解析错误

        // cout << content << endl;

        // 解析报文
        Request req;
        // 反序列化
        r = req.Deserialization(content);
        if(!r) return ""; 

        // cout << req._x << req._op << req._y << endl;
        // 计算结果
        Response resp = CaculatorHelper(req);

        content = "";
        // 序列化
        resp.Serialization(&content);
        // 添加报头
        content = Encode(content);
        return content;
    }

    ~FuncServer()
    { }
};
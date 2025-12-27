#include <iostream>
#include <memory>
#include "TcpServer.hpp"
#include "Protocol.hpp"
#include "Caculator.hpp"

using namespace std;

Calculator calculator;

void DefaultOnMessage(weak_ptr<Connection> conn)
{
    if (conn.expired())
        return;
    auto connection_ptr = conn.lock();
    // 对报文进行处理
    cout << "上层得到了数据： " << connection_ptr->Inbuffer() << endl;
    string response_str = calculator.Handler(connection_ptr->Inbuffer()); // 我们的业务逻辑比较简单，没有特别耗时的操作
    if (response_str.empty())
        return;
    // lg(Debug, "%s", response_str.c_str());
    // response_str 发送出去
    connection_ptr->AppendOutBuffer(response_str);
    // 正确的理解发送？
    // connection_ptr->_send_cb(connection_ptr);

    auto tcpserver = connection_ptr->_tcp_svr_ptr.lock();
    tcpserver->Sender(connection_ptr);
}

int main()
{
    auto svr = make_shared<TcpServer>(8080, DefaultOnMessage);

    svr->Init();
    svr->Loop();

    return 0;
}
#pragma once
#include "Socket.hpp"
#include "log.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <pthread.h>
#include <sstream>
#include <fstream>

using namespace std;

const uint16_t defaultport = 8080;
const string response_blank = "\r\n";  // 响应空行
const string homepage = "index.html";  // 网页首页 
const string defaultpath = "./wwwroot";// 默认路径



extern log lg;

// ==================== 线程信息 ====================
class ThreadData
{
public:
    ThreadData(int fd)
    :_sockfd(fd)
    { }
public:
    int _sockfd;
};

// ==================== 处理HTTP请求 ====================
class HttpRequset
{
public:

    // ########################## 提取报头和正文参数 ##########################
    // [方法] [url] [版本]
    void Deserialize(string req)
    {
        while(true)
        {
            size_t pos = req.find(response_blank);
            if(pos == string::npos) break; // 找到结尾了

            // 找到一行的结尾，把这一行插入vector中，并进行擦除
            string temp = req.substr(0, pos);
            _headline.push_back(temp);
            // 注意擦除的时候应该把"\r\n"也擦掉
            req.erase(0, pos + sizeof(response_blank));
        }
        // 剩下的都是正文部分
        _text = req;
    }


    void Parse()
    {
        // stringstream 解析字符串
        stringstream ss(_headline[0]);
        ss >> _method >> _url >> _httpVersion;
        file_path = defaultpath;
        // 请求的时候没有指定路径或要求访问首页
        if(_url == "/" || _url == "index.html") 
        {
            file_path += "/";
            file_path += homepage;
        }
        else
        {
            file_path += _url;
        }
    }

public: 
    vector<string> _headline;   // 报头
    string _text;               // 正文部分

    // 解析出来的信息
    string _method;
    string _url;
    string _httpVersion;
    string file_path; // 访问时的页面路径
};

// ==================== HTTP服务器 ====================
class HttpServer
{
public:
    HttpServer(uint16_t port = defaultport)
    :_port(port)
    { }


    // ########################## 线程执行 ##########################
    static void* threadRun(void* args)
    {
        pthread_detach(pthread_self());
        ThreadData* td = static_cast<ThreadData*>(args);
        
        HandlerHttp(td->_sockfd);

        delete td;
        return nullptr;
    }


    // ########################## 构建http响应 ##########################
    static void HandlerHttp(int sockfd)
    {
        char buffer[10240];
        ssize_t n = recv(sockfd, buffer, sizeof(buffer), 0);
        if(n > 0)
        {
            buffer[n] = 0;
            // cout << buffer << endl;
            // 假设现在读到的就是一个完成的请求
            HttpRequset req;
            req.Deserialize(buffer);
            req.Parse();
            

            // 返回响应
            string response_text = ReadHtmlContent(req.file_path);         // 响应正文 

            string response_line = "HTTP/1.0 200 OK\r\n"; // 响应行

            string response_header = "Content-length: ";  // 报头
            response_header += to_string(response_text.size());
            response_header += "\r\n";

            // 构建响应
            string response = response_line;
            response += response_header;
            response += response_blank;
            response += response_text;

            send(sockfd, response.c_str(), response.size(), 0);
        }
        close(sockfd);
    }

   


    // ########################## 返回所需要的网页 ##########################
    static string ReadHtmlContent(const std::string &htmlpath)
    {
        ifstream in(htmlpath, ios::binary);
        if(!in.is_open()) return "";

        in.seekg(0, std::ios_base::end);
        auto len = in.tellg();
        in.seekg(0, std::ios_base::beg);

        std::string content;
        content.resize(len);

        in.read((char*)content.c_str(), content.size());
        //std::string content;
        //std::string line;
        //while(std::getline(in, line))
        //{
        //    content += line;
        //}

        in.close();
        return content;
    }


    // ########################## 服务器启动 ##########################
    void Start()
    {
        _listensock.Socket();
        // 防止服务器立即重启失败
        int opt = 1;
        setsockopt(_listensock.GetFd(), SOL_SOCKET, SO_REUSEADDR|SO_REUSEPORT, &opt, sizeof(opt));

        _listensock.Bind(_port);
        _listensock.Listen();
        while(1)
        {
            // 获取客户端的信息
            string clientip;
            uint16_t clientport;
            // 阻塞式的等待链接
            int sockfd = _listensock.Accept(&clientip, &clientport);
            if(sockfd < 0)  continue;
            lg(Info, "Get a new connect - [%s:%d]", clientip.c_str(), clientport);

            ThreadData* td = new ThreadData(sockfd);

            pthread_t tid;
            pthread_create(&tid, nullptr, threadRun, td);
        }
    }


    ~HttpServer()
    { }

public:
    uint16_t _port;
    Sock _listensock;
};
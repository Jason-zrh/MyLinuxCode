#pragma once
#include "Socket.hpp"
#include "Daemon.hpp"
#include "log.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include <pthread.h>

using namespace std;

const string response_blank = "\r\n";  // 响应空行
const string homepage = "index.html";  // 网页首页 
const string defaultpath = "./wwwroot";// 默认路径

extern log lg;



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


    // GET /index.html HTTP/1.1 (请求行)
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
          // 拼接目标网址
            file_path += _url;
        }

        size_t pos = file_path.rfind(".");
        if(pos == string :: npos)
            _suffix = ".html";
        else
            _suffix = file_path.substr(pos);

    }

public: 
    vector<string> _headline;   // 报头
    string _text;               // 正文部分

    // 解析出来的信息
    string _method;
    string _url;
    string _httpVersion;
    string _suffix;               
    string file_path;           // 访问时的页面路径
};

class HttpServer;

// ==================== 线程信息 ====================
class ThreadData
{
public:
    ThreadData(int fd, HttpServer* s)
    :_sockfd(fd)
    ,_svr(s)
    { }
public:
    int _sockfd;
    HttpServer* _svr;
};

// ==================== HTTP服务器 ====================
class HttpServer
{
public:
    HttpServer(uint16_t port)
    :_port(port)
    { 
        _content_type[".html"] = "text/html";
        _content_type[".png"] = "image/png";
        _content_type[".jpg"] = "image/jpg";

    }


    // ########################## 线程执行 ##########################
    static void* threadRun(void* args) // 线程执行函数必须是static方法否则会导致参数里有this指针
    {
        // 线程解挂接，当线程结束后自动回收线程
        pthread_detach(pthread_self());
        ThreadData* td = static_cast<ThreadData*>(args);
        td->_svr->HandlerHttp(td->_sockfd);
        delete td;
        return nullptr;
    }


    // ########################## 构建http响应 ##########################
    void HandlerHttp(int sockfd)
    {
        char buffer[10240];
        ssize_t n = recv(sockfd, buffer, sizeof(buffer), 0);
        if(n > 0)
        {
            // 假设现在读到的就是一个完成的请求
            buffer[n] = 0;
            cout << buffer << endl;
            
            // 构建请求解析对象
            HttpRequset req;
            req.Deserialize(buffer);
            req.Parse();
            

            // 构建响应
            bool isExist = true;
            string response_text;
            response_text = ReadHtmlContent(req.file_path);         // 响应正文
            if(response_text.empty())
            {
                isExist = false;
                string err_html = defaultpath;
                err_html += "/";
                err_html += "err.html";
                response_text = ReadHtmlContent(err_html);
            }

            // 进行差错处理
            string response_line;
            if(isExist)
            {
                response_line = "HTTP/1.0 200 OK\r\n";              // 响应行
            }
            else
            {
                response_line = "HTTP/1.0 404 Not Found\r\n";
            }

            string response_header = "Content-length: ";            // 报头
            response_header += to_string(response_text.size());
            response_header += "\r\n";

            response_header += "Content-Type: ";                    // 响应类型
            response_header += SuffixToDesc(req._suffix);
            response_header += "\r\n";


            // 构建响应
            string response = response_line;                        // 响应行
            response += response_header;                            // 报头
            response += response_blank;                             // 空行
            response += response_text;                              // 响应正文

            send(sockfd, response.c_str(), response.size(), 0);
        }
        close(sockfd);
    }

    // 对应一下返回正文的类型
    string SuffixToDesc(const string &suffix)
    {
        auto iter = _content_type.find(suffix);
        if(iter == _content_type.end()) 
            return _content_type[".html"];
        else 
            return _content_type[suffix];
    }


    // ########################## 返回所需要的网页 ##########################
    string ReadHtmlContent(const std::string &htmlpath)
    {
        // 打开文件(以二进制形式，方便读取图片等格式)
        ifstream in(htmlpath, ios::binary);

        // 目标路径下文件不存在返回空串
        if(!in.is_open()) 
            return "";

        // 计算文件的长度的方式: 先把标志位移动到文件的结尾，保存大小，再把标志位移回文件开头
        in.seekg(0, std::ios_base::end);
        auto len = in.tellg();
        in.seekg(0, std::ios_base::beg);

        std::string content;
        content.resize(len);

        // 从打开的文件中读取
        in.read((char*)content.c_str(), content.size());

        in.close();
        return content;
    }


    // ########################## 服务器启动 ##########################
    void Start()
    {
        // 加入守护进程
        // Daemon();

        // 创建套接字
        _listensock.Socket();
        // 防止服务器立即重启失败
        int opt = 1;
        setsockopt(_listensock.GetFd(), SOL_SOCKET, SO_REUSEADDR|SO_REUSEPORT, &opt, sizeof(opt));
        // 绑定套接字
        _listensock.Bind(_port);
        // 监听
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

            ThreadData* td = new ThreadData(sockfd, this);

            pthread_t tid;
            pthread_create(&tid, nullptr, threadRun, td);
        }
    }


    ~HttpServer()
    { }

public:
    uint16_t _port;
    Sock _listensock;
    unordered_map<string, string> _content_type;
};
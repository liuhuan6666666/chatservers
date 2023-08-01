#ifndef CHATSERVER_H
#define CAHTSERVER_H
#include <muduo/net/TcpServer.h>
#include<muduo/net/EventLoop.h>
#include<iostream>
#include<string>
using namespace std;
using namespace muduo;
using namespace muduo::net;
class Chatserver{
    public:
    Chatserver(EventLoop* loop,
            const InetAddress& listenAddr,
            const string& nameArg);
    void start();
    private:
        void onConnection(const TcpConnectionPtr&);
        void onMessage(const TcpConnectionPtr&,
                            Buffer*,
                            Timestamp);
        TcpServer _server;
        EventLoop * _loop;
    
};
#endif
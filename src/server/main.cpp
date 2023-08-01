#include"../../include/server/chatserver.hpp"
#include"../../include/server/chatservice.hpp"
#include<iostream>
#include<signal.h>
using namespace std;
//处理服务器ctrl+c结束后，重置user的状态信息
void resetHandler(int){
    Chatservice::instance()->reset();
    exit(0);
}

int main(int argc, char **argv){
    signal(SIGINT,resetHandler);//信号优先级最高
    // 解析通过命令行参数传递的ip和port
    char *ip = argv[1];
    uint16_t port = atoi(argv[2]);
    EventLoop loop;
    InetAddress addr(ip,port);
    Chatserver server(&loop,addr,"ChatServer");
    server.start();
    loop.loop();
    return 0;
}
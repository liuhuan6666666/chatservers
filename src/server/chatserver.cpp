#include"../../include/server/chatserver.hpp"
#include"../../include/server/chatservice.hpp"
#include<string>
#include"../../thirdparty/json.hpp"
using json=nlohmann::json;
Chatserver::Chatserver(EventLoop* loop,
            const InetAddress& listenAddr,
            const string& nameArg):_server(loop,listenAddr,nameArg),_loop(loop){
                //注册链接回调
                _server.setConnectionCallback(std::bind(&Chatserver::onConnection,this,_1));
                //注册消息回调
                _server.setMessageCallback(std::bind(&Chatserver::onMessage,this,_1,_2,_3));
                //设置线程个数
                _server.setThreadNum(4);
            }
void Chatserver::start(){
    _server.start();
}
void Chatserver::onConnection(const TcpConnectionPtr& conn){
    if(!conn->connected()){
        Chatservice::instance()->clientCloseEXception(conn);//客户端异常退出
        //客户端断开连接
        conn->shutdown();
    }
}
void Chatserver::onMessage(const TcpConnectionPtr&conn,
                            Buffer*buf,
                            Timestamp time){
    cout<<time.toString()<<endl;
    //使用json接收的信息是字符或者字符串流
    string buff=buf->retrieveAllAsString();
    //数据的反序列化
    json js=json::parse(buff);
    //达到目的：完全解耦网络模块的代码和业务模块的代码
    //通过js["msgid"] 获取业务handler=》conn,解析的js对象，time
    auto msgHandler= Chatservice::instance()->getHandler(js["msgid"].get<int>());//js["msgid"].get<int>()将json数据类型转化成int类型
    //回调消息绑定好的事件处理器，来执行相应的业务处理
    msgHandler(conn,js,time);
}

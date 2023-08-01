#ifndef CHATSERVICE_H
#define CHATSERVICE_H
#include<unordered_map>
#include<muduo/net/TcpConnection.h>
#include<functional>
#include<mutex>
#include"../../thirdparty/json.hpp"
#include"./redis/redis.hpp"
#include"model/groupmodel.hpp"
#include "model/offlinemessagemodel.hpp"
#include"model/friendmodel.hpp"
#include "model/UserModel.hpp"
using namespace std;
using namespace muduo;
using namespace muduo::net;
using json=nlohmann::json;
//处理消息事件方法的类型
using MsgHandler = std::function<void(const TcpConnectionPtr& conn, json &js,Timestamp time)>;
//聊天服务器业务类
class Chatservice{
public:
    /*
    单例对象是一种设计模式，在该模式中，一个类只允许创建一个实例，并提供一个全局的访问点。这意味着任何时候访问该对象时，都只能获得同一个实例。
    在C++中，可以通过静态成员变量和静态成员函数来实现单例对象。
    Chatservice* Chatservice::instance = nullptr;
    */
    //获取单例对象的接口函数
    static Chatservice* instance();
    //处理登录用户
    void login(const TcpConnectionPtr& conn, json &js,Timestamp time);
    //处理注册用户
    void reg(const TcpConnectionPtr& conn, json &js,Timestamp time);
    //1V1聊天业务
    void oneChat(const TcpConnectionPtr& conn, json &js,Timestamp time);
    //添加好友
    void addFriend(const TcpConnectionPtr& conn, json &js,Timestamp time);
    //创建群组
    void creategroup(const TcpConnectionPtr& conn, json &js,Timestamp time);
    //加入群组业务
    void addGroupuser(const TcpConnectionPtr& conn, json &js,Timestamp time);
    //群聊天
    void groupchat(const TcpConnectionPtr& conn, json &js,Timestamp time);
    //获取消息对应的处理器
    MsgHandler getHandler(int msgid);
    // 从redis消息队列中获取订阅的消息
    void handleRedisSubscribeMessage(int userid, string msg);
    //处理客户端异常退出
    void clientCloseEXception(const TcpConnectionPtr& conn);
    //服务器异常退出
    void reset();
    //账户注销
    void loginout(const TcpConnectionPtr &conn, json &js, Timestamp time);
private:;
    Chatservice();
    //存储消息id和对应的业务处理方法
    unordered_map<int,MsgHandler> _msgHandlerMap;
    //存储在线用户的通信连接
    unordered_map<int,TcpConnectionPtr> _userConnmap;
    //定义互斥锁,保证_userConnmap线程的安全
    mutex _connmutex;
    //数据操作类对象
    UserModel _usermodel;
    //消息操作类对象
    offLineMsgemodel _offlineMsgmodel;
    //好友操作类对象
    friendmodel _friendmodel;
    //群组对象
    GroupModel _groupmodel;

    //redis操作对象
    Redis _redis;
};


#endif
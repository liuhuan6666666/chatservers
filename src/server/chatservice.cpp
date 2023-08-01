#include "../../include/server/chatservice.hpp"
#include "../../include/public.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <muduo/base/Logging.h>
using namespace std;
using namespace muduo;
// 单例模式
Chatservice *Chatservice::instance()
{
    static Chatservice service;
    return &service;
}

// 注册消息以及对应的回调函数
Chatservice::Chatservice()
{
    _msgHandlerMap[LOGIN_MSG] = std::bind(&Chatservice::login, this, _1, _2, _3); //_1占位符
    _msgHandlerMap[REG_MSG] = std::bind(&Chatservice::reg, this, _1, _2, _3);
    _msgHandlerMap[ONE_CHAT_MSG] = std::bind(&Chatservice::oneChat, this, _1, _2, _3);
    _msgHandlerMap[ADD_FRIEND_MSG] = std::bind(&Chatservice::addFriend, this, _1, _2, _3);
    _msgHandlerMap[CREATE_GROUP_MSG] = std::bind(&Chatservice::creategroup, this, _1, _2, _3);
    _msgHandlerMap[ADD_GROUP_MSG] = std::bind(&Chatservice::addGroupuser, this, _1, _2, _3);
    _msgHandlerMap[GROUP_CHAT_MSG] = std::bind(&Chatservice::groupchat, this, _1, _2, _3);
    _msgHandlerMap[LOGINOUT_MSG] = std::bind(&Chatservice::loginout, this, _1, _2, _3);
    if (_redis.connect())
    {
        // 设置上报消息的回调
        _redis.init_notify_handler(std::bind(&Chatservice::handleRedisSubscribeMessage, this, _1, _2));
    }
}

// 获取消息对应的处理器
MsgHandler Chatservice::getHandler(int msgid)
{
    // 记录错误日志
    auto it = _msgHandlerMap.find(msgid);
    if (it == _msgHandlerMap.end())
    {
        // 返回一个默认的处理器,空操作
        return [=](const TcpConnectionPtr &conn, json &js, Timestamp time)
        {
            LOG_ERROR << "msgid:" << msgid << " can not find handler!";
        };
    }
    else
    {
        return _msgHandlerMap[msgid];
    }
}

// 处理登录用户  业务不能直接处理数据，应该封装新的对象   ORM 业务层操作的都是对象 DAO
void Chatservice::login(const TcpConnectionPtr &conn, json &js, Timestamp time)
{ // id pwd
    int id = js["id"].get<int>();
    string pwd = js["password"];
    User user = _usermodel.query(id); // 返回主键的数据
    if (user.getId() == id && user.getPwd() == pwd)
    {
        if (user.getState() == "online")
        {
            // 登录重复
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 2;
            response["errmsg"] = "chongfu";
            conn->send(response.dump()); // 将数据结构（如字典、列表等）序列化为 JSON 字符串
        }
        else
        {
            {
                lock_guard<mutex> lock(_connmutex); // lock_guard初始化加锁，lock_guard析构解锁
                // 登陆成功，记录用户连接信息
                _userConnmap[id] = conn;
            } // 加大括号代表作用域，出了就解锁

            // 服务器订阅redis通道
            _redis.subscribe(id);

            // 登录成功，更新用户状态信息
            user.setState("online");
            _usermodel.updatestate(user);
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 0; // 为0说明成功
            response["id"] = user.getId();
            response["name"] = user.getName();
            // 查询用户是否有离线消息
            vector<string> vec = _offlineMsgmodel.query(id);
            if (!vec.empty())
            {
                response["offlinemsg"] = vec; // 序列化，可以直接将数组放入里面
                // 读取离线消息后，删除离线消息
                _offlineMsgmodel.remove(id);
            }
            // 查询好友信息
            vector<User> uservec = _friendmodel.query(id);
            if (!uservec.empty())
            {
                vector<string> vec2;
                for (User &user : uservec)
                {
                    json js;
                    js["id"] = user.getId();
                    js["name"] = user.getName();
                    js["state"] = user.getState();
                    vec2.push_back(js.dump());
                }
                response["friends"] = vec2;
            }
            // 查询群组信息
            vector<Group> groupvec = _groupmodel.queryGroups(id);
            if (!groupvec.empty())
            {
                vector<string> vec1;
                for (Group &group : groupvec)
                {
                    json js;
                    js["id"] = group.getId();
                    js["groupname"] = group.getName();
                    js["groupdesc"] = group.getDesc();
                    vector<string> vec2;
                    for (GroupUser &user : group.getUsers())
                    {
                        json jss;
                        jss["id"] = user.getId();
                        jss["name"] = user.getName();
                        jss["state"] = user.getState();
                        jss["role"] = user.getRole();
                        vec2.push_back(jss.dump());
                    }
                    js["users"] = vec2;
                    vec1.push_back(js.dump());
                }
                response["groups"] = vec1;
            }
            conn->send(response.dump()); // 将数据结构（如字典、列表等）序列化为 JSON 字符串
        }
    }
    else
    {
        // 登录失败，用户存在密码错误或者用户不存在
        json response;
        response["msgid"] = LOGIN_MSG_ACK;
        response["errno"] = 1;
        response["errmsg"] = "usrid or password is invalid!";
        conn->send(response.dump()); // 将数据结构（如字典、列表等）序列化为 JSON 字符串
    }
}

// 处理注册用户  name passward
void Chatservice::reg(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    string name = js["name"];
    string pwd = js["password"];
    User user;
    user.setName(name);
    user.setPwd(pwd);
    bool state = _usermodel.insert(user);
    if (state)
    {
        // 注册成功
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 0; // 为0说明成功
        response["id"] = user.getId();
        conn->send(response.dump()); // 将数据结构（如字典、列表等）序列化为 JSON 字符串
    }
    else
    {
        // 注册成功
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 1;       // 为0说明成功
        conn->send(response.dump()); // 将数据结构（如字典、列表等）序列化为 JSON 字符串
    }
}

// 创建群组
void Chatservice::creategroup(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    cerr << userid << endl;
    string name = js["groupname"];
    string desc = js["groupdesc"];
    Group group(-1, name, desc);
    if (_groupmodel.createGroup(group))
    {
        _groupmodel.addGroupuser(userid, group.getId(), "creator"); // 是将user放入groupusers组中
    }
}

// 加入群组业务
void Chatservice::addGroupuser(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    _groupmodel.addGroupuser(userid, groupid, "normal");
}

// 添加好友  msgid id friend
void Chatservice::addFriend(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int friendid = js["friendid"].get<int>();
    // 存储好友信息
    _friendmodel.insert(userid, friendid);
}

// 单个聊天
void Chatservice::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int toid = js["toid"].get<int>();
    cout << toid << endl;
    {
        lock_guard<mutex> lock(_connmutex);
        auto it = _userConnmap.find(toid);

        if (it != _userConnmap.end()) // toid在一个服务器上
        {
            cout << "liuhuan" << endl;
            // 在线， 服务器主动推送消息给toid用户
            it->second->send(js.dump());
            return;
        }
    }
    // id与toid不在一个服务器上
    User user = _usermodel.query(toid);
    if (user.getState() == "online")
    {
        _redis.publish(toid, js.dump());
        return;
    }
    // 不在线，存储离线消息
    _offlineMsgmodel.insert(toid, js.dump());
}

// 群聊天
void Chatservice::groupchat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    vector<int> useridvec = _groupmodel.queryGroupUsers(userid, groupid);
    lock_guard<mutex> lock(_connmutex);
    for (int id : useridvec)
    {
        auto it = _userConnmap.find(id); // 从已经连接的用户map中找用户
        if (it != _userConnmap.end())
        {
            it->second->send(js.dump());
        }
        else
        { // id与toid不在一个服务器上
            User user = _usermodel.query(id);
            if (user.getState() == "online")
            {
                _redis.publish(id, js.dump());
            }
            _offlineMsgmodel.insert(id, js.dump()); // 用户离线，先数据存储在sql表格中
        }
    }
}
// 从redis消息队列中获取订阅的消息
void Chatservice::handleRedisSubscribeMessage(int userid, string msg)
{   //因为从redis将数据返回到订阅该id的服务器，那么该id一定在该服务器上
    json js=json::parse(msg.c_str());
    lock_guard<mutex> lock(_connmutex);
    auto it=_userConnmap.find(userid);
    if(it!=_userConnmap.end()){
        it->second->send(js.dump());
        return;
    }
    //用户不在线
    _offlineMsgmodel.insert(userid,msg);
}

// 客户端异常退出
void Chatservice::clientCloseEXception(const TcpConnectionPtr &conn)
{
    User user;
    {
        lock_guard<mutex> lock(_connmutex); // lock_guard初始化加锁，lock_guard析构解锁
        for (auto it = _userConnmap.begin(); it != _userConnmap.end(); it++)
        {
            if (it->second == conn)
            {
                user.setId(it->first);
                // 从map表删除连接信息
                _userConnmap.erase(it);
                break;
            }
        }
    }
    // 异常退出取消订约
    _redis.unsubscribe(user.getId());
    // 更新用户状态信息
    if (user.getId() != -1)
    {
        user.setState("offline");
        _usermodel.updatestate(user);
    }
}

// 客户端账户注销
void Chatservice::loginout(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    User user;
    int userid = js["id"].get<int>();
    user.setId(userid);
    {
        lock_guard<mutex> lock(_connmutex);
        auto it = _userConnmap.find(userid);
        if (it != _userConnmap.end())
        {
            _userConnmap.erase(it);
        }
    }
    // 在redis中取消订阅
    _redis.unsubscribe(userid);
    // 更新用户状态信息
    if (userid != -1)
    {
        user.setState("offline");
        _usermodel.updatestate(user);
    }
}
// 服务器异常退出
void Chatservice::reset()
{
    // 把online的用户设置成offline
    _usermodel.resetState();
}
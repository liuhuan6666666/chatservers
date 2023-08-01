#ifndef PUBLIC_H
#define PUBLIC_H
/*
server和client的公共文件
*/
enum EnMsgType{  //enum是一种用来定义枚举类型的关键字  EnMsgType是一个自定义的枚举类型，用于表示消息的类型
    LOGIN_MSG=1,//登录消息
    LOGIN_MSG_ACK,//登录响应消息             
    REG_MSG,    //注册消息
    REG_MSG_ACK,   //注册成功响应id
    ONE_CHAT_MSG,  //聊天消息
    ADD_FRIEND_MSG, //添加好友消息
    LOGINOUT_MSG,//客户端退出登录

    CREATE_GROUP_MSG,//创建群组
    ADD_GROUP_MSG,//加入群组
    GROUP_CHAT_MSG,//群聊天
};
#endif
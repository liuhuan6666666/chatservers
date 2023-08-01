#ifndef FRIENDMODEL_H
#define FRIENDMODEL_H
#include"user.hpp"
#include<vector>
using namespace std;
class friendmodel
{
public:
    // 添加好友关系
    void insert(int userid, int friendid);
    // 返回用户好友列表  friendid 以及它的信息
    vector<User> query(int userid);
};

#endif
#include "../../include/server/model/UserModel.hpp"
#include "../../include/server/db/db.h"
#include <iostream>
using namespace std;
// user表增加
bool UserModel::insert(User &user)
{
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into users(name,passward,state) values('%s','%s','%s')", user.getName().c_str(), user.getPwd().c_str(), user.getState().c_str());
    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            // 获取插入成功的用户数据生成的主键id
            user.setId(mysql_insert_id(mysql.getconnection())); // id是自增长，所以数据库中不用设置
            return true;
        }
    }
    return false;
}

User UserModel::query(int id)
{
    User user;
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select * from users where id = %d", id);
    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row = mysql_fetch_row(res); // 读取一行
            if (row != nullptr)
            {
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setPwd(row[2]);
                user.setState(row[3]);
                mysql_free_result(res);
                return user;
            }
        }
    }
    return user;
}

// 更新用户状态信息
bool UserModel::updatestate(User user)
{
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "update users set state='%s' where id = %d", user.getState().c_str(), user.getId());
    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            return true;
        }
    }
    return false;
}

// 重置用户状态信息
void UserModel::resetState()
{
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "update users set state='%s' where state = 'online'", "offline");
    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}
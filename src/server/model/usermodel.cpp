#include "../../../include/server/model/UserModel.hpp"
#include <iostream>
using namespace std;
// user表增加
bool UserModel::insert(User &user)
{
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into users(name,passward,state) values('%s','%s','%s')", user.getName().c_str(), user.getPwd().c_str(), user.getState().c_str());
    MYSQL *mysql;
    SqlConnRAII(&mysql, SqlConnPool::Instance());
    if (SqlConnPool::Instance()->update(mysql, sql))
    {
        // 获取插入成功的用户数据生成的主键id
        user.setId(mysql_insert_id(mysql)); // mysql_insert_id:返回最后插入值的ID 值
        SqlConnPool::Instance()->FreeConn(mysql);
        return true;
    }
    SqlConnPool::Instance()->FreeConn(mysql);
    return false;
}

User UserModel::query(int id)
{
    User user;
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select * from users where id = %d", id);
    MYSQL *mysql;
    SqlConnRAII(&mysql, SqlConnPool::Instance());
    MYSQL_RES *res = SqlConnPool::Instance()->query(mysql, sql);
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
        }
    }
    SqlConnPool::Instance()->FreeConn(mysql);
    return user;
}

// 更新用户状态信息
bool UserModel::updatestate(User user)
{
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "update users set state='%s' where id = %d", user.getState().c_str(), user.getId());
    MYSQL *mysql;
    SqlConnRAII(&mysql, SqlConnPool::Instance());
    if (SqlConnPool::Instance()->update(mysql, sql))
    {   
        SqlConnPool::Instance()->FreeConn(mysql);
        return true;
    }
    SqlConnPool::Instance()->FreeConn(mysql);
    return false;
}

// 重置用户状态信息
void UserModel::resetState()
{
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "update users set state='%s' where state = 'online'", "offline");
    MYSQL *mysql;
    SqlConnRAII(&mysql, SqlConnPool::Instance());
    SqlConnPool::Instance()->update(mysql, sql);
    SqlConnPool::Instance()->FreeConn(mysql);
}
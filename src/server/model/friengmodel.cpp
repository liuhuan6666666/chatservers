#include "../../include/server/model/friendmodel.hpp"
#include "../../include/server/db/db.h"
#include "../../include/server/model/UserModel.hpp"
#include <vector>
// 添加好友关系
void friendmodel::insert(int userid, int friendid)
{
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into  Friend(userid,friendid) values('%d','%d')", userid, friendid);
    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}
// 返回用户好友列表  friendid 以及它的信息
vector<User> friendmodel::query(int userid)
{
    vector<User> v1;
    // 组装sql语句
    char sql[1024] = {0};
    // 先从Friend中取出userid的所有信息，在内连users取出userid的所有信息
    sprintf(sql, "select a.id,a.name,a.state from users a inner join Friend b on b.friendid=a.id where b.userid = %d", userid);
    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql); // 一定要释放资源
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                v1.push_back(user);
            }
            mysql_free_result(res);
        }
    }
    return v1;
}
#include "../../../include/server/model/offlinemessagemodel.hpp"
#include "../../../include/server/db/sqlconnRALL.hpp"

// 存储用户离线消息
void offLineMsgemodel::insert(int userid, string msg)
{
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into OfflineMessage(userid,message) values('%d','%s')", userid, msg.c_str());
    MYSQL *mysql;
    SqlConnRAII(&mysql, SqlConnPool::Instance());
    SqlConnPool::Instance()->update(mysql, sql);
    SqlConnPool::Instance()->FreeConn(mysql);
}

// 删除用户离线消息
void offLineMsgemodel::remove(int userid)
{
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "delete from OfflineMessage where userid =%d", userid);
    MYSQL *mysql;
    SqlConnRAII(&mysql, SqlConnPool::Instance());
    SqlConnPool::Instance()->update(mysql, sql);
    SqlConnPool::Instance()->FreeConn(mysql);
}
// 查询用户离线 消息
vector<string> offLineMsgemodel::query(int userid)
{
    vector<string> v1;
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select message from OfflineMessage where userid = %d", userid);
    MYSQL *mysql;
    SqlConnRAII(&mysql, SqlConnPool::Instance());
    MYSQL_RES *res = SqlConnPool::Instance()->query(mysql,sql); // 一定要释放资源
    if (res != nullptr)
    {
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res)) != nullptr)
        {
            v1.push_back(row[0]);
        }
        mysql_free_result(res);
    }
    SqlConnPool::Instance()->FreeConn(mysql);
    return v1;
}

#include "../../../include/server/model/group.hpp"
#include "../../../include/server/model/groupmodel.hpp"
#include "../../../include/server/model/groupuser.hpp"
#include "../../../include/server/db/sqlconnRALL.hpp"
// 创建群组
bool GroupModel::createGroup(Group &group)
{
    char sql[1024] = {0};
    sprintf(sql, "insert into ALLGroup(groupname, groupdesc) values('%s', '%s')",
            group.getName().c_str(), group.getDesc().c_str());
    MYSQL *mysql;
    SqlConnRAII(&mysql, SqlConnPool::Instance());
    if (SqlConnPool::Instance()->update(mysql, sql))
    {
        group.setId(mysql_insert_id(mysql));
        SqlConnPool::Instance()->FreeConn(mysql);
        return true;
    }
    SqlConnPool::Instance()->FreeConn(mysql);
    return false;
}
// 加入数组
void GroupModel::addGroupuser(int userid, int groupid, string role)
{
    char sql[1024] = {0};
    sprintf(sql, "insert into GroupUser(groupid,userid,grouprole) values('%d','%d','%s')", groupid, userid, role.c_str());
    MYSQL *mysql;
    SqlConnRAII(&mysql, SqlConnPool::Instance());
    SqlConnPool::Instance()->update(mysql, sql);
    SqlConnPool::Instance()->FreeConn(mysql);
}
// 查询用户所在群组信息
vector<Group> GroupModel::queryGroups(int userid)
{
    char sql[1024] = {0};
    sprintf(sql, "select a.id,a.groupname,a.groupdesc from ALLGroup a inner join GroupUser b on a.id=b.groupid where b.userid=%d", userid);
    vector<Group> groupVec;
    MYSQL *mysql;
    SqlConnRAII(&mysql, SqlConnPool::Instance());
    MYSQL_RES *res = SqlConnPool::Instance()->query(mysql, sql);
    if (res != nullptr)
    {
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res)) != nullptr)
        {
            Group group;
            group.setId(atoi(row[0]));
            group.setName(row[1]);
            group.setDesc(row[2]);
            groupVec.push_back(group);
        }
        mysql_free_result(res);
    }
    for (Group &group : groupVec)
    {
        sprintf(sql, "select a.id,a.name,a.state,b.grouprole from users a inner join GroupUser b on a.id=b.userid where b.groupid=%d", group.getId());
        MYSQL_RES *res = SqlConnPool::Instance()->query(mysql, sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                GroupUser user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                user.setRole(row[3]);
                group.getUsers().push_back(user);
            }
            mysql_free_result(res);
        }
    }
    SqlConnPool::Instance()->FreeConn(mysql);
    return groupVec;
}
// 根据指定的groupid查询用户id列表，除userid自己，主要用户群聊业务给群组其他成员群发消息
vector<int> GroupModel::queryGroupUsers(int userid, int groupid)
{
    char sql[1024] = {0};
    sprintf(sql, "select userid from GroupUser where groupid=%d and userid!=%d", groupid, userid);
    vector<int> groupVec;
    MYSQL *mysql;
    SqlConnRAII(&mysql, SqlConnPool::Instance());
    MYSQL_RES *res = SqlConnPool::Instance()->query(mysql, sql);
    if (res != nullptr)
    {
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res)) != nullptr)
        {
            groupVec.push_back(atoi(row[0]));
        }
        mysql_free_result(res);
    }
    SqlConnPool::Instance()->FreeConn(mysql);
    return groupVec;
}
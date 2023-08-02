#ifndef SQLCONNPOOL_H
#define SQLCONNPOOL_H
#include <mysql/mysql.h>
#include <string>
#include <queue>
#include <mutex>
#include <semaphore.h>
#include <thread>
#include <cassert>
#include<muduo/base/Logging.h>
using namespace std;
class SqlConnPool {
public:
    static SqlConnPool *Instance();

    MYSQL *GetConn();
    void FreeConn(MYSQL * conn);
    int GetFreeConnCount();

    void Init(int connSize);
    void ClosePool();
    bool update(MYSQL* _conn,string sql);
    MYSQL_RES* query(MYSQL* _conn,string sql);
private:
    SqlConnPool();
    ~SqlConnPool();

    int MAX_CONN_;
    int useCount_;
    int freeCount_;

    std::queue<MYSQL *> connQue_;  //mysql线程池，就是一个队列，队列中存储的是mysql的指针
    std::mutex mtx_;
    sem_t semId_;  //信号量
};


#endif // SQLCONNPOOL_H
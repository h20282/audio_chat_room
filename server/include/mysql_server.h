/*
 * @Author: FengYanBin
 * @Date: 2021-08-10 09:49:20
 * @LastEditors: FengYanBin
 * @LastEditTime: 2021-08-10 10:33:09
 * @Description: file content
 * @FilePath: /server/include/mysql_server.h
 */
#ifndef __MYSQL_SERVER__H
#define __MYSQL_SERVER__H
#include "pack_def.h"
#include <mysql/mysql.h>
#include <list>
#include <string>

using namespace std;

class CMysql
{
public:
    int ConnectMysql(char *server, char *user, char *password, char *database);
    int SelectMysql(char *szSql, int nColumn, list<string> &lst);
    int UpdataMysql(char *szsql);
    void DisConnect();

private:
    MYSQL *conn;
    MYSQL_RES *results;
    MYSQL_ROW record;
};

#endif

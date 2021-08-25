/*
 * @Author: FengYanBin
 * @Date: 2021-08-10 10:01:41
 * @LastEditors: FengYanBin
 * @LastEditTime: 2021-08-10 11:52:18
 * @Description: file content
 * @FilePath: /server/src/mysql_server.cpp
 */
#include "../include/mysql_server.h"

int CMysql::ConnectMysql(char *server, char *user, char *password, char *database)
{
    conn = NULL;
    conn = mysql_init(NULL);
    mysql_set_character_set(conn, "gb2312");
    if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0))
    {
        return false;
    }
    return true;
}

//查询mysql语句存在list中。
int CMysql::SelectMysql(char *szSql, int nColumn, list<string> &lst)
{
    results = NULL;
	mysql_query(conn, "set names utf8");
    if (mysql_query(conn, szSql))
        return false;
    results = mysql_store_result(conn);
    if (NULL == results)
        return false;
    while ((record = mysql_fetch_row(results)))
    {

        for (int i = 0; i < nColumn; i++)
        {
            lst.push_back(record[i]);
        }
    }
    return true;
}

int CMysql::UpdataMysql(char *szsql)
{
    if (!szsql)
        return false;
    if (mysql_query(conn, szsql))
        return false;

    return true;
}

void CMysql::DisConnect()
{
    mysql_close(conn);
}

#pragma once
#include <mysql/mysql.h>
#include <string>
#include <iostream>
#include <vector>
//#include <exception>
class tinyMysql
{
public:
    bool excuteAndReturnOneRow(const std::string& sql,std::vector<std::string>& result);
    bool excuteAndReturnNULL(const std::string& sql);
    tinyMysql(std::string& db_host,std::string& db_user, std::string& db_passwd,std::string& db,unsigned int port);
    ~tinyMysql();
private:
    MYSQL* _mysql;
    //MYSQL_RES* _res;
};
/*
class tinyMysqlException: public
{
    const char* what() const thow(){
        return "tinyMysql ERROR!";
    }
}
*/
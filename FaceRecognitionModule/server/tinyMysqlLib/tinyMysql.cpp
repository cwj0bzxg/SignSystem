#include "tinyMysql.h"
#include <mysql/mysql.h>
#include <string>
#include <iostream>
#include <vector>
#include <memory>  // 使用智能指针管理MYSQL_RES

tinyMysql::tinyMysql(std::string& db_host,std::string& db_user, std::string& db_passwd,std::string& db,unsigned int port)
{
    _mysql = mysql_init(NULL);
    mysql_real_connect(_mysql,db_host.c_str(),db_user.c_str(),db_passwd.c_str(),db.c_str(),port,NULL,0);
    if(_mysql){
        std::cout<<"connect to mysql success"<<std::endl;
    }
    else{
        std::cout<<"connect to mysql failed"<<std::endl;
    }
}

tinyMysql::~tinyMysql()
{
    if(_mysql!=NULL){
        mysql_close(_mysql);
    }
}

bool tinyMysql::excuteAndReturnOneRow(const std::string& sql,std::vector<std::string>& result)
{
    // 用于select操作
    // 本函数要求输入的sql语句至多只返回一行
    bool isTrue = false;
    result.clear();
    mysql_query(_mysql, sql.c_str());
    // 使用智能指针会发生段错误，我没弄清楚是为什么
    //std::shared_ptr<MYSQL_RES> auto_res(mysql_store_result(_mysql),mysql_free_result);
    MYSQL_RES* _res = mysql_store_result(_mysql);
    if(_res == NULL){
        return false;
    }
    std::shared_ptr<MYSQL_RES> auto_res(_res,mysql_free_result);
    unsigned int res_rows = mysql_num_rows(auto_res.get());
    if(res_rows == 0){
        isTrue = true;
    }
    else if(res_rows == 1){
        isTrue = true;
        MYSQL_ROW row = mysql_fetch_row(auto_res.get());
        unsigned int num_fields = mysql_num_fields(auto_res.get());
        unsigned int i;
        for(i=0;i<num_fields;i++){
            result.push_back(row[i]);
        }
    }
    //mysql_free_result(_res);
    return isTrue;
}

bool tinyMysql::excuteAndReturnNULL(const std::string& sql)
{
    // 用于insert操作
    mysql_query(_mysql, sql.c_str());
    if(mysql_affected_rows(_mysql)>0){
        return true;
    }
    else{
        return false;
    }
}

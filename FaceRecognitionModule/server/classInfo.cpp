#include "boost/asio.hpp"
#include "boost/bind.hpp"
#include <string>
#include <iostream>
#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <memory> // 使用智能指针，管理mysql c api 中的指针和内存
#include <stdio.h>
#include <mysql/mysql.h>
#include <string.h>
#include "tinyMysql.h"
#include <vector>
using namespace boost::asio;

static std::string registerOntheServer(tinyMysql& itemMysql,std::string className,std::string time, std::string label){

    std::string sql = "select * from student_course where c_id = "+ className +" and s_id = "+ label + ";";
    std::cout<<" sql : "<<sql<<std::endl;
    std::vector<std::string> result;
    std::string res="NULL";
    if(itemMysql.excuteAndReturnOneRow(sql,result)){
        int n=result.size();
        if(n!=2){
            return res;
        }else{
            res = result[0];
        }
        std::string sql2 = "insert into attendance(s_id,c_id,time,result) values("+ label +"," + className + ",'" + time + "','signed');";
        std::cout<<" 检测到本班成员,记入数据库 \n"
                 <<"sql2 : "<<sql2
                 <<std::endl;
        if(itemMysql.excuteAndReturnNULL(sql2)){
            std::cout<<result[0]<<" 登记成功 "<<std::endl;
        }
        else{
            std::cout<<result[0]<<" 登记失败 "<<std::endl;
        }
    }
    else{
        std::cout<<" 检测到非本班成员 "<<std::endl;
    }
    return res;
}

io_service service;

void handle_connection(std::string& host, std::string& user, std::string& passwd, 
	std::string& db, unsigned int port)
{

    tinyMysql itemMysql(host, user, passwd, db, port);

    ip::tcp::acceptor acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), 8001));
    char buff[1024];
    while(true){  // 只能同时处理一个请求
        std::cout<<"正在等待新的请求"<<std::endl;
        ip::tcp::socket sock(service);
        acceptor.accept(sock);

        boost::asio::streambuf b;    // 接收json
        boost::asio::streambuf::mutable_buffers_type bufs = b.prepare(512);
        std::istream is(&b);
        std::string strContent,className, descript, label;
        size_t n = sock.receive(bufs);
        std::cout<<" n : "<<n<<std::endl;
        b.commit(n);

        boost::property_tree::ptree pt;     // 解析json
        boost::property_tree::json_parser::read_json(is,pt);
        className=pt.get<std::string>("className");
        descript=pt.get<std::string>("descript");
        label=pt.get<std::string>("label");
        std::cout<<"className : "<<className<<" descript : "<<descript<<" label : "<<label<<std::endl;

        boost::asio::streambuf a;  // 将json中的数据传给数据库
        std::ostream os(&a);       // 并将结果返回
        os << registerOntheServer(itemMysql,className,descript,label);
        size_t m = sock.send(a.data());
        b.consume(m);

        sock.close();
        std::cout<<"请求结束"<<std::endl;
    }
}

int main(int argc, char** argv)
{
    if(argc!=4){
        std::cout<<"您需要输入3个参数\n"
                 <<"./classInfo <user | 数据库用户名> <passwd | 数据库密码> <db | 使用的数据库名称(face_student)>\n"
                 <<"本程序使用xxx.sql来构建数据库\n"
                 <<"本程序将表student_record和course_info存在face_student数据库中\n"
                 <<"(本程序默认使用本地的mysql数据库,端口号为3306)\n"
                 <<std::endl;
        return 2;
    }
    std::string host = "localhost";
    std::string user = argv[1];
    std::string passwd = argv[2];
	std::string db = argv[3];
    unsigned int port = 3306;
    std::cout<<"单线程服务器启动"<<std::endl;
    handle_connection(host, user, passwd, db, port);
    return 0;
}
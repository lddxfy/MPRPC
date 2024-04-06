#include "test.pb.h"
#include <iostream>
#include <string>
using namespace lddxfy;

int main()
{
    //封装数据
    // LoginRequest req;
    // req.set_name("zhang san");
    // req.set_pwd("123456");
    // //对象数据序列化  =》  char*
    // std::string send_str;
    // if (req.SerializeToString(&send_str))
    // {
    //     std::cout << send_str.c_str() << std::endl;
    // }

    // //反序列化
    // LoginRequest reqB;
    // if(reqB.ParseFromString(send_str))
    // {
    //     std::cout<<reqB.name()<<std::endl;
    //      std::cout<<reqB.pwd()<<std::endl;
    // }


    // LoginResponse rep;
    // ResultCode* rescode = rep.mutable_res();
    // rescode->set_errcode(2);
    // rescode->set_errmsg("密码错误");
    // std::string send_str;


    GetUserListResponse rep;
    User * user = rep.add_user();
    user->set_name("zhang san");
    user->set_age(21);
    user->set_sex(User::MAN);

    User * user1 = rep.add_user();
    user1->set_name("zhang san");
    user1->set_age(21);
    user1->set_sex(User::MAN);


    std::cout<<rep.user_size()<<std::endl;




    // if(rep.SerializeToString(&send_str))
    // {
    //     std::cout<<rep.res().errmsg()<<std::endl;
    // }
    

    return 0;
}
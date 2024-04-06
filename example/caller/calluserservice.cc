#include <iostream>
#include "mprpcapplication.h"
#include "user.pb.h"
#include "mprpcchannel.h"

int main(int argc,char ** argv)
{
    MprpcApplication::Init(argc,argv);
    // 演示调用远程发布的rpc方法Login
    lddxfy::UserServiceRPC_Stub stub(new MprpcChannel());
    // rpc方法的请求参数
    lddxfy::LoginRequest request;
    request.set_name("zhang san");
    request.set_pwd("123456");
    // rpc方法的响应
    lddxfy::LoginResponse response;
    // 发起rpc方法的调用  同步的rpc调用过程  MprpcChannel::callmethod
    stub.Login(nullptr,&request,&response,nullptr);
     // 一次rpc调用完成，读调用的结果
    if(0 == response.res().errcode())
    {
        std::cout << "rpc login response success:" << response.success() << std::endl;
    }
    else
    {
        std::cout << "rpc login response error : " << response.res().errmsg() << std::endl;
    }
    return 0;
}


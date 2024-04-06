#include <iostream>
#include "user.pb.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"
using namespace std;

class UserService : public lddxfy::UserServiceRPC // 使用在rpc服务发布端
{
public:
    bool Login(string name, string pwd)
    {
        cout << "local login" << endl;
        cout<<"name:"<<name<<"pwd:"<<pwd<<endl;
        return false;
    }

    void Login(::google::protobuf::RpcController *controller,
               const ::lddxfy::LoginRequest *request,
               ::lddxfy::LoginResponse *response,
               ::google::protobuf::Closure *done)
    {
        //获取远程数据
        string name = request->name();
        string pwd = request->pwd();
        //做本地业务
        bool res = Login(name,pwd);
        //把响应写入
        lddxfy::ResultCode * code = response->mutable_res();
        code->set_errcode(1);
        code->set_errmsg("error login!");
        response->set_success(res);
        //执行回调      响应对象的序列化和网路发送（框架完成）
        done->Run();
    }

private:
};

int main(int argc,char **argv)
{
    MprpcApplication::Init(argc,argv);

    //rpcprovide是一个rpc网络服务对象，把UserService对象发布到rpc节点上
    RpcProvider rpcprovide;
    rpcprovide.NotifyService(new UserService());


    //启动一个rpc服务发布节点    Run以后，进程进入阻塞状态，等待远程的rpc调用请求
    rpcprovide.Run();
    return 0;
}
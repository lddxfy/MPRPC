#include <iostream>
#include "friend.pb.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"
#include <vector>
using namespace std;

class FriendService : public lddxfy::FriendServiceRPC
{
public:
    vector<string> GetFriendList(int userid)
    {
        vector<string> res;
        cout << "local GetFriendList" << endl;
        cout<<"userid:"<<userid<<endl;
        res.push_back("cjq");
        res.push_back("ltq");
        res.push_back("hy");
        return res;
    }

    void GetFriendList(::google::protobuf::RpcController* controller,
                       const ::lddxfy::GetFriendListRequest* request,
                       ::lddxfy::GetFriendListResponse* response,
                       ::google::protobuf::Closure* done)
    {
        uint32_t userid = request->userid();
        vector<string> vec = GetFriendList(userid);
        response->mutable_res()->set_errcode(0);
        response->mutable_res()->set_errmsg("");
        for(string v : vec)
        {
            response->add_friends(v);
        }
        done->Run();

    }




};

int main(int argc,char **argv)
{
    MprpcApplication::Init(argc,argv);

    //rpcprovide是一个rpc网络服务对象，把UserService对象发布到rpc节点上
    RpcProvider rpcprovide;
    rpcprovide.NotifyService(new FriendService());


    //启动一个rpc服务发布节点    Run以后，进程进入阻塞状态，等待远程的rpc调用请求
    rpcprovide.Run();
    return 0;
}
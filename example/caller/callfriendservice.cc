#include <iostream>
#include "mprpcapplication.h"
#include "mprpcchannel.h"
#include "friend.pb.h"

int main(int argc, char **argv)
{
    MprpcApplication::Init(argc, argv);
    // 演示调用远程发布的rpc方法Login
    lddxfy::FriendServiceRPC_Stub stub(new MprpcChannel());
    // rpc方法的请求参数
    lddxfy::GetFriendListRequest request;
    request.set_userid(20);
    // rpc方法的响应
    lddxfy::GetFriendListResponse response;

    MprpcController controller;
    // 发起rpc方法的调用  同步的rpc调用过程  MprpcChannel::callmethod
    stub.GetFriendList(&controller, &request, &response, nullptr);
    // 一次rpc调用完成，读调用的结果
    if (controller.Failed())
    {
        std::cout << controller.ErrorText() << std::endl;
    }
    else
    {
        if (0 == response.res().errcode())
        {
            std::cout << "rpc GetFriendList success :" << std::endl;
            int size = response.friends_size();
            for (int i = 0; i < size; i++)
            {
                std::cout << i + 1 << ": friend name :" << response.friends(i) << std::endl;
            }
        }
        else
        {
            std::cout << "rpc GetFriendList response error : " << response.res().errmsg() << std::endl;
        }
    }

    return 0;
}
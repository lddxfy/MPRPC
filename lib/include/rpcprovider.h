#pragma

#include "google/protobuf/service.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/TcpServer.h"
#include "logger.h"
#include <unordered_map>


class RpcProvider
{
public:

    void NotifyService(google::protobuf::Service *service);

    void Run();

    
private:
    muduo::net::EventLoop loop;
    //有新的socket连接过来，设置连接回调
    void onConnection(const muduo::net::TcpConnectionPtr&);
    //有消息传递设置回调
    void onMessage(const muduo::net::TcpConnectionPtr&,muduo::net::Buffer*,muduo::Timestamp);
    struct ServiceInfo
    {
        google::protobuf::Service *m_service; // 保存服务对象;
        std::unordered_map<std::string,const google::protobuf::MethodDescriptor*> ServiceMethodMap;
    };
    std::unordered_map<std::string,ServiceInfo> ServiceMap;

    void SendRpcResponse(const muduo::net::TcpConnectionPtr &conn,google::protobuf::Message *response);
};
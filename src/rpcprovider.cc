#include "rpcprovider.h"
#include "mprpcapplication.h"
#include "rpcheader.pb.h"
#include "zookeeperutil.h"
#include <functional>
#include <google/protobuf/descriptor.h>

void RpcProvider::NotifyService(google::protobuf::Service *service)
{

    ServiceInfo serviceinfo;
    // 获取了服务对象的描述信息
    const google::protobuf::ServiceDescriptor *servicedesc = service->GetDescriptor();
    // 获取服务对象的名称
    std::string service_name = servicedesc->name();
    serviceinfo.m_service = service;
    // 获取服务对象service的方法的数量
    int methodCnt = servicedesc->method_count();
    //std::cout << "service name:" << service_name << std::endl;
    LOG_INFO("service name: %s",service_name.c_str());
    for (int i = 0; i < methodCnt; i++)
    {
        const google::protobuf::MethodDescriptor *methoddesc = servicedesc->method(i);
        std::string method_name = methoddesc->name();
        //std::cout << "method name:" << method_name << std::endl;
        LOG_INFO("method name: %s",method_name.c_str());
        serviceinfo.ServiceMethodMap.insert({method_name, methoddesc});
    }
    ServiceMap.insert({service_name, serviceinfo});
}
// 启动rpc服务节点，开始提供rpc远程网络调用服务
void RpcProvider::Run()
{
    std::string ip = MprpcApplication::GetInstance().GetConfig().Load("mprpcserverip");
    uint16_t port = atoi(MprpcApplication::GetInstance().GetConfig().Load("mprpcserverport").c_str());
    muduo::net::InetAddress address(ip, port);

    muduo::net::TcpServer server(&loop, address, "RpcProvider");

    server.setConnectionCallback(bind(&RpcProvider::onConnection, this, std::placeholders::_1));
    server.setMessageCallback(bind(&RpcProvider::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    server.setThreadNum(4);

    // 把当前rpc节点上要发布的服务全部注册到zk上面，让rpc client可以从zk上发现服务
    // session timeout   30s     zkclient 网络I/O线程  1/3 * timeout 时间发送ping消息
    ZkClient zkCli;
    zkCli.Start();
    // service_name为永久性节点    method_name为临时性节点
    for(auto &sp : ServiceMap)
    {
        std::string service_path = "/" + sp.first;
        zkCli.Create(service_path.c_str(),nullptr,0);
        for(auto &mp : sp.second.ServiceMethodMap)
        {
            // /service_name/method_name   /UserServiceRpc/Login 存储当前这个rpc服务节点主机的ip和port
            std::string  method_path = service_path + "/" + mp.first;
            char data[64] = {0};
            sprintf(data,"%s:%d",ip.c_str(),port);
            // ZOO_EPHEMERAL表示znode是一个临时性节点
            zkCli.Create(method_path.c_str(),data,strlen(data),ZOO_EPHEMERAL);
        }
    }

    //std::cout << "server RpcProvider ip port: " << server.ipPort() << std::endl;
    LOG_INFO("server RpcProvider ip port: %s",server.ipPort().c_str());
    server.start();
    loop.loop();
}

void RpcProvider::onConnection(const muduo::net::TcpConnectionPtr &conn)
{
    if (!conn->connected())
    {
        //std::cout << "disconnection" << std::endl;
        LOG_ERROR("disconnection");
        conn->shutdown();
    }
}
// 有消息传递设置回调  远程发起一个rpc请求，框架负责对其进行解析
// RpcProvider和RpcConsumer协商好之间通信用的protobuf数据类型
// service_name method_name args_size避免连包问题    定义proto的message类型，进行数据头的序列化和反序列化
// service_name method_name 相当于header_str
// header_size(4个字节) + header_str + args_str
void RpcProvider::onMessage(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buf, muduo::Timestamp)
{
    // 网络上接收的远程rpc调用请求的字符流    Login args
    std::string recvbuf = buf->retrieveAllAsString();

    // 从字符流中读取前4个字节的内容
    uint32_t header_size = 0;
    recvbuf.copy((char*)&header_size,4,0);

    // 根据header_size读取数据头的原始字符流，反序列化数据，得到rpc请求的详细信息
    std::string rpc_header_str = recvbuf.substr(4, header_size);
    mprpc::RpcHeader rpcHeader;
    std::string service_name;
    std::string method_name;
    uint32_t args_size;
    if (rpcHeader.ParseFromString(rpc_header_str))
    {
        service_name = rpcHeader.service_name();
        method_name = rpcHeader.method_name();
        args_size = rpcHeader.args_size();
    }
    else
    {
        // 数据头反序列化失败
        //std::cout << "rpc_header_str:" << rpc_header_str << " parse error!" << std::endl;
        LOG_ERROR("rpc_header_str: %s,parse error!",rpc_header_str.c_str());
        return;
    }

    // 获取rpc方法参数的字符流数据
    std::string args_str = recvbuf.substr(4 + header_size, args_size);
    // 打印调试信息
    std::cout << "============================================" << std::endl;
    std::cout << "header_size: " << header_size << std::endl;
    std::cout << "rpc_header_str: " << rpc_header_str << std::endl;
    std::cout << "service_name: " << service_name << std::endl;
    std::cout << "method_name: " << method_name << std::endl;
    std::cout << "args_str: " << args_str << std::endl;
    std::cout << "============================================" << std::endl;

    // 获取service对象和method对象
    auto it = ServiceMap.find(service_name);
    if (it == ServiceMap.end())
    {
        std::cout << service_name << " is not exist!" << std::endl;
        return;
    }

    auto mit = it->second.ServiceMethodMap.find(method_name);
    if (mit == it->second.ServiceMethodMap.end())
    {
        //std::cout << service_name << ":" << method_name << " is not exist!" << std::endl;
        LOG_ERROR("%s : %s is not exist",service_name.c_str(),method_name.c_str());
        return;
    }
    // 获取service对象  new UserService
    google::protobuf::Service *service = it->second.m_service;
    // 获取method对象  Login
    const google::protobuf::MethodDescriptor *mehtond = mit->second;
    // 生成rpc方法调用的请求request和响应response参数
    google::protobuf::Message *request = service->GetRequestPrototype(mehtond).New();
    if (!request->ParseFromString(args_str))
    {
        std::cout << "request parse error, content:" << args_str << std::endl;
        LOG_ERROR("request parse error, content: %s",args_str.c_str());
        return;
    }
    google::protobuf::Message *response = service->GetResponsePrototype(mehtond).New();

    // 给下面的method方法的调用，绑定一个Closure的回调函数
    google::protobuf::Closure *done = google::protobuf::NewCallback<RpcProvider,
                                                                    const muduo::net::TcpConnectionPtr &,
                                                                    google::protobuf::Message *>(this, &RpcProvider::SendRpcResponse, conn, response);

    // 在框架上根据远端rpc请求，调用当前rpc节点上发布的方法
    // new UserService().Login(controller, request, response, done)
    service->CallMethod(mehtond, nullptr, request, response, done);
}
// Closure的回调操作，用于序列化rpc的响应和网络发送
void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr &conn, google::protobuf::Message *response)
{
    std::string response_str;
    if(response->SerializeToString(&response_str))
    {
        // 序列化成功后，通过网络把rpc方法执行的结果发送会rpc的调用方
        conn->send(response_str);
    }
    else
    {
        //std::cout << "serialize response_str error!" << std::endl;
        LOG_ERROR("serialize response_str error!");
    }
    conn->shutdown(); // 模拟http的短链接服务，由rpcprovider主动断开连接
}
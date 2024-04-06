#pragma once
#include <zookeeper/zookeeper.h>
#include <semaphore.h>
#include <string>
// 封装的zk客户端类
class ZkClient
{
public:
    ZkClient();
    ~ZkClient();
    // zkclient启动连接zkserver
    void Start();
    // 在zkserver上根据指定的path创建znode节点
    void Create(const char *path,const char* data,int datalen,int statee = 0);
    // 根据参数指定的znode节点路径，或者znode节点的值
    std::string GetDate(const char *path);
private:
    zhandle_t *m_zhandle;
};
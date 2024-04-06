#pragma once
#include "mprpcconfig.h"
#include "mprpccontroller.h"
// mprpc框架的基础类，负责框架的一些初始化操作
class MprpcApplication
{
public:
    static void Init(int argc,char** argv);
    static MprpcApplication& GetInstance();
    MprpcConfig& GetConfig();
private:
    MprpcApplication(){};
    MprpcApplication(const MprpcApplication&) = delete;
    MprpcApplication(const MprpcApplication&&) = delete;
    static MprpcConfig rpcconfig;
};
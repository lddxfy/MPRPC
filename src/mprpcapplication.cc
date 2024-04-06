#include "mprpcapplication.h"
#include <iostream>
#include <unistd.h>
using namespace std;

MprpcConfig MprpcApplication::rpcconfig;

void ShowArgsHelp()
{
    cout<<"格式如下：command -i <configfile>"<<endl;
}

void MprpcApplication::Init(int argc, char **argv)
{
    if(argc < 2)
    {
        ShowArgsHelp();
        exit(EXIT_FAILURE);
    }

    int c = 0;
    string config_file;
    while ((c = getopt(argc,argv,"i:"))!=-1)
    {
        switch (c)
        {
        case 'i':
            config_file = optarg;
            break;
        case '?':
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        case ':':
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        default:
            break;
        }
    }
    rpcconfig.LoadConfigFile(config_file.c_str());
    // cout<<rpcconfig.Load("rpcserverip")<<endl;
    // cout<<rpcconfig.Load("rpcserverport")<<endl;
    
}


MprpcApplication& MprpcApplication::GetInstance()
{
    static MprpcApplication app;
    return app;
};

MprpcConfig& MprpcApplication::GetConfig()
{
    return rpcconfig;
}
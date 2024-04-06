#pragma once
#include <iostream>
#include <unordered_map>
#include <string>
class MprpcConfig
{
public:
    void LoadConfigFile(const char * config_file);

    std::string Load(const std::string &key);
private:
    std::unordered_map<std::string,std::string> map;
    void Trim(std::string &src_buf);
};
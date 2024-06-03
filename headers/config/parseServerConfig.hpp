#pragma once
#include <config/ServerConfig.hpp>
#include <yaml/MappingNode.hpp>

namespace webserv
{

ServerConfig parseServerConfig(const yaml::MappingNode &node, const std::string &yamlFilePath);

}	 // namespace webserv

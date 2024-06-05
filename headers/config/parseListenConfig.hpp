#pragma once
#include <config/ListenConfig.hpp>
#include <yaml/MappingNode.hpp>

namespace webserv
{

ListenConfig parseListenConfig(const yaml::MappingNode &node, const std::string &yamlFilePath);

}	 // namespace webserv

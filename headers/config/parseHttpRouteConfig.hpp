#pragma once
#include <config/HttpRouteConfig.hpp>
#include <yaml/MappingNode.hpp>

namespace webserv
{

HttpRouteConfig parseHttpRouteConfig(const yaml::MappingNode &node, const std::string &yamlFilePath);

}	 // namespace webserv

#pragma once
#include <config/CgiConfig.hpp>
#include <yaml/MappingNode.hpp>

namespace webserv
{

CgiConfig parseCgiConfig(const yaml::MappingNode &node);

}	 // namespace webserv

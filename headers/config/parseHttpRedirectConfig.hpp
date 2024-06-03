#pragma once
#include <config/HttpRedirectConfig.hpp>
#include <yaml/MappingNode.hpp>

namespace webserv
{

HttpRedirectConfig parseHttpRedirectConfig(const yaml::MappingNode &node);

}	 // namespace webserv

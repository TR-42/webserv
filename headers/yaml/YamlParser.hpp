#pragma once

#include <Logger.hpp>
#include <iostream>
#include <string>
#include <yaml/MappingNode.hpp>
#include <yaml/NodeBase.hpp>
#include <yaml/ScalarNode.hpp>

namespace webserv
{

namespace yaml
{

bool parse(std::istream &yamlStream, MappingNode &root, const Logger &logger);
bool parse(const std::string &yamlStr, MappingNode &root, const Logger &logger);
bool parse(const std::vector<std::string> &yamlLines, MappingNode &root, const Logger &logger);

}	 // namespace yaml

}	 // namespace webserv

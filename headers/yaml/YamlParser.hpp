#pragma once

#include <Logger.hpp>
#include <iostream>
#include <string>
#include <yaml/MappingNode.hpp>
#include <yaml/NodeBase.hpp>
#include <yaml/ScalarNode.hpp>
#include <yaml/SequenceNode.hpp>

namespace webserv
{

namespace yaml
{

class YamlParser
{
 public:
	static NodePtr parse(std::istream &yamlStream);

 private:
	YamlParser(const Logger &logger);
	YamlParser(const YamlParser &other);
	static NodePtr parseNode(const std::string &yaml, size_t &index);
	static ScalarNode parseScalar(const std::string &yaml, size_t &index);
	static NodePtr parseMap(const std::string &yaml, size_t &index);
	static NodePtr parseList(const std::string &yaml, size_t &index);
	static void skipSpaces(const std::string &yaml, size_t &index);
	static void skipSpacesAndCommas(const std::string &yaml, size_t &index);
	static void skipSpacesAndCommasAndColons(const std::string &yaml, size_t &index);
	const Logger &logger;
};

}	 // namespace yaml

}	 // namespace webserv

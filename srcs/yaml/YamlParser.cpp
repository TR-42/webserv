#include "yaml/YamlParser.hpp"

#include <Logger.hpp>
#include <utils/pickLine.hpp>
#include <vector>
#include <yaml/NodeBase.hpp>
#include <yaml/ScalarNode.hpp>

#include "utils/splitNameValue.hpp"

namespace webserv
{

namespace yaml
{

YamlParser::YamlParser(const Logger &logger) : logger(logger) {}

NodePtr YamlParser::parse(std::istream &yamlStream)
{
	MappingNode *root = new MappingNode("root");
	std::string yaml;
	while (std::getline(yamlStream, yaml)) {
		size_t index = 0;
		// 一旦スカラーノードのみ
		ScalarNode node = parseScalar(yaml, index);
		root->addNode(node);
	}
	return (NodePtr)(root);
};

void YamlParser::skipSpaces(const std::string &yaml, size_t &index)
{
	while (index < yaml.size() && yaml[index] == ' ') {
		++index;
	}
};

ScalarNode YamlParser::parseScalar(const std::string &yamlRowLine, size_t &index)
{
	skipSpaces(yamlRowLine, index);
	std::string lineWithoutSpaces = yamlRowLine.substr(index);

	std::pair<std::string, std::string> nameValue = utils::splitNameValue(lineWithoutSpaces);
	return ScalarNode(nameValue.first, nameValue.second);
};

}	 // namespace yaml

}	 // namespace webserv

#include "yaml/YamlParser.hpp"

#include <Logger.hpp>
#include <sstream>
#include <utils/pickLine.hpp>
#include <vector>
#include <yaml/NodeBase.hpp>
#include <yaml/ScalarNode.hpp>

#include "utils/splitNameValue.hpp"

namespace webserv
{

namespace yaml
{

static bool parse(
	MappingNode &parent,
	const std::vector<std::string> &yamlLines,
	size_t &yamlLinesIndex,
	size_t indentLevel,
	const Logger &logger
);

bool parse(std::istream &yamlStream, MappingNode &root, const Logger &logger)
{
	std::vector<std::string> yamlLines;
	std::string yaml;
	while (std::getline(yamlStream, yaml)) {
		yamlLines.push_back(yaml);
	}
	return parse(yamlLines, root, logger);
}

bool parse(const std::string &yamlStr, MappingNode &root, const Logger &logger)
{
	std::istringstream yamlStream(yamlStr);
	return parse(yamlStream, root, logger);
}

bool parse(const std::vector<std::string> &yamlLines, MappingNode &root, const Logger &logger)
{
	size_t yamlLinesIndex = 0;
	return parse(root, yamlLines, yamlLinesIndex, 0, logger);
}

static size_t countSpace(const std::string &yaml)
{
	size_t count = 0;
	while (count < yaml.size() && yaml[count] == ' ') {
		++count;
	}
	return count;
};

static bool parse(
	MappingNode &parent,
	const std::vector<std::string> &yamlLines,
	size_t &yamlLinesIndex,
	size_t indentLevel,
	const Logger &logger
)
{
	MappingNode lastMappingNode("");
	for (; yamlLinesIndex < yamlLines.size(); ++yamlLinesIndex) {
		const std::string &yamlRowLine = yamlLines[yamlLinesIndex];
		if (yamlRowLine.empty()) {
			continue;
		}

		size_t count = countSpace(yamlRowLine);
		if (count == yamlLines[yamlLinesIndex].size()) {
			continue;
		}

		if (indentLevel < count) {
			if (lastMappingNode.getKey().empty()) {
				return false;
			}
			if (!parse(lastMappingNode, yamlLines, yamlLinesIndex, count, logger)) {
				return false;	 // 直上のノードに追加するのに失敗した時
			}
			parent.addNode(lastMappingNode);
			lastMappingNode = MappingNode("");
			if (yamlLinesIndex + 1 < yamlLines.size()) {
				size_t nextCount = countSpace(yamlLines[yamlLinesIndex + 1]);
				if (indentLevel < nextCount) {
					return false;
				}
			}
			continue;
		}

		if (!lastMappingNode.getKey().empty()) {
			parent.addNode(lastMappingNode);
			lastMappingNode = MappingNode("");
		}

		if (count < indentLevel) {	// 別のparentになる 兄弟ノードかそれ以上の親ノードになる
			--yamlLinesIndex;
			return true;
		}

		std::string lineWithoutSpaces = yamlRowLine.substr(count);

		std::pair<std::string, std::string> nameValue = utils::splitNameValue(lineWithoutSpaces);
		if (!nameValue.second.empty()) {
			ScalarNode node(nameValue.first, nameValue.second);
			parent.addNode(node);
		} else {
			lastMappingNode = MappingNode(nameValue.first);
		}
	}

	if (!lastMappingNode.getKey().empty()) {
		parent.addNode(lastMappingNode);
	}

	return true;
}

}	 // namespace yaml

}	 // namespace webserv

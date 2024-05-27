#include "yaml/YamlParser.hpp"

#include <gtest/gtest.h>

#include <sstream>

namespace webserv
{

namespace yaml
{

TEST(YamlParserScalar, Empty)
{
	std::stringstream ss("");
	NodePtr node = YamlParser::parse(ss);
	ASSERT_NE(node, nullptr);
	const MappingNode *mappingNode = dynamic_cast<const MappingNode *>(node);
	ASSERT_NE(mappingNode, nullptr);
	ASSERT_EQ(mappingNode->getNodes().size(), 0);
}

}	 // namespace yaml

}	 // namespace webserv

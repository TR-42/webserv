#include "yaml/YamlParser.hpp"

#include <Logger.hpp>
#include <cstddef>

#include "gtest/gtest.h"

namespace webserv
{

namespace yaml
{

TEST(YamlParser, empty)
{
	std::stringstream ss("");
	MappingNode root("");
	Logger logger;
	ASSERT_EQ(parse(ss, root, logger), true);
	EXPECT_EQ(root.getNodes().size(), 0);
}

TEST(YamlParser, ScalarNode)
{
	std::string yamlStr = "key: value\n";
	MappingNode root("");
	Logger logger;
	ASSERT_EQ(parse(yamlStr, root, logger), true);
	EXPECT_EQ(root.getNodes().size(), 1);
	if (root.getNodes().size() == 0)
		return;
	EXPECT_EQ(root.getNodes().at(0)->getKey(), "key");
	const ScalarNode *scalarNode = dynamic_cast<const ScalarNode *>(root.getNodes().at(0));
	EXPECT_NE(scalarNode, nullptr);
	if (scalarNode == nullptr)
		return;
	EXPECT_EQ(scalarNode->getValue(), "value");
}

}	 // namespace yaml

}	 // namespace webserv

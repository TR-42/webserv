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

TEST(YamlParser, MappingNode)
{
	std::vector<std::string> yamlLines = {
		"key:",
		"  key1: value1",
		"  key2: value2",
	};
	MappingNode root("");
	Logger logger;
	EXPECT_TRUE(parse(yamlLines, root, logger));
	EXPECT_EQ(root.getNodes().size(), 1);
	if (root.getNodes().size() == 0)
		return;
	EXPECT_EQ(root.getNodes().at(0)->getKey(), "key");
	const MappingNode *mappingNode = dynamic_cast<const MappingNode *>(root.getNodes().at(0));
	EXPECT_NE(mappingNode, nullptr);
	if (mappingNode == nullptr)
		return;
	EXPECT_EQ(mappingNode->getNodes().size(), 2);
	if (mappingNode->getNodes().size() < 1)
		return;
	const ScalarNode *scalarNode1 = dynamic_cast<const ScalarNode *>(mappingNode->getNodes().at(0));
	EXPECT_NE(scalarNode1, nullptr);
	if (scalarNode1 != nullptr) {
		EXPECT_EQ(scalarNode1->getKey(), "key1");
		EXPECT_EQ(scalarNode1->getValue(), "value1");
	}
	if (mappingNode->getNodes().size() < 2)
		return;
	const ScalarNode *scalarNode2 = dynamic_cast<const ScalarNode *>(mappingNode->getNodes().at(1));
	EXPECT_NE(scalarNode2, nullptr);
	if (scalarNode2 != nullptr) {
		EXPECT_EQ(scalarNode2->getKey(), "key2");
		EXPECT_EQ(scalarNode2->getValue(), "value2");
	}
}

}	 // namespace yaml

}	 // namespace webserv

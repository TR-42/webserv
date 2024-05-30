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

TEST(YamlParser, ComplexNode)
{
	std::vector<std::string> yamlLines = {
		"key0:",
		"  key1: value1",
		"  key2:",
		"    key3: value3",
		"    key4: value4",
		"key5:",
		" key6: value6",
		" key7: value7",
	};
	MappingNode root("");
	Logger logger;
	EXPECT_TRUE(parse(yamlLines, root, logger));
	EXPECT_EQ(root.getNodes().size(), 2);
	if (root.getNodes().size() == 0)
		return;
	EXPECT_EQ(root.getNodes().at(0)->getKey(), "key0");
	const MappingNode *mappingNode0 = dynamic_cast<const MappingNode *>(root.getNodes().at(0));
	EXPECT_NE(mappingNode0, nullptr);
	if (mappingNode0 != nullptr) {
		EXPECT_EQ(mappingNode0->getNodes().size(), 2);
		if (1 <= mappingNode0->getNodes().size()) {
			const ScalarNode *scalarNode1 = dynamic_cast<const ScalarNode *>(mappingNode0->getNodes().at(0));
			EXPECT_NE(scalarNode1, nullptr);
			if (scalarNode1 != nullptr) {
				EXPECT_EQ(scalarNode1->getKey(), "key1");
				EXPECT_EQ(scalarNode1->getValue(), "value1");
			}
		}
		if (2 <= mappingNode0->getNodes().size()) {
			const MappingNode *mappingNode2 = dynamic_cast<const MappingNode *>(mappingNode0->getNodes().at(1));
			EXPECT_NE(mappingNode2, nullptr);
			if (mappingNode2 != nullptr) {
				EXPECT_EQ(mappingNode2->getNodes().size(), 2);
				if (1 <= mappingNode2->getNodes().size()) {
					const ScalarNode *scalarNode3 = dynamic_cast<const ScalarNode *>(mappingNode2->getNodes().at(0));
					EXPECT_NE(scalarNode3, nullptr);
					if (scalarNode3 != nullptr) {
						EXPECT_EQ(scalarNode3->getKey(), "key3");
						EXPECT_EQ(scalarNode3->getValue(), "value3");
					}
				}
				if (2 <= mappingNode2->getNodes().size()) {
					const ScalarNode *scalarNode4 = dynamic_cast<const ScalarNode *>(mappingNode2->getNodes().at(1));
					EXPECT_NE(scalarNode4, nullptr);
					if (scalarNode4 != nullptr) {
						EXPECT_EQ(scalarNode4->getKey(), "key4");
						EXPECT_EQ(scalarNode4->getValue(), "value4");
					}
				}
			}
		}
	}

	if (root.getNodes().size() < 2)
		return;

	EXPECT_EQ(root.getNodes().at(1)->getKey(), "key5");
	const MappingNode *mappingNode5 = dynamic_cast<const MappingNode *>(root.getNodes().at(1));
	EXPECT_NE(mappingNode5, nullptr);
	if (mappingNode5 != nullptr) {
		EXPECT_EQ(mappingNode5->getNodes().size(), 2);
		if (1 <= mappingNode5->getNodes().size()) {
			const ScalarNode *scalarNode6 = dynamic_cast<const ScalarNode *>(mappingNode5->getNodes().at(0));
			EXPECT_NE(scalarNode6, nullptr);
			if (scalarNode6 != nullptr) {
				EXPECT_EQ(scalarNode6->getKey(), "key6");
				EXPECT_EQ(scalarNode6->getValue(), "value6");
			}
		}
		if (2 <= mappingNode5->getNodes().size()) {
			const ScalarNode *scalarNode7 = dynamic_cast<const ScalarNode *>(mappingNode5->getNodes().at(1));
			EXPECT_NE(scalarNode7, nullptr);
			if (scalarNode7 != nullptr) {
				EXPECT_EQ(scalarNode7->getKey(), "key7");
				EXPECT_EQ(scalarNode7->getValue(), "value7");
			}
		}
	}
}

TEST(YamlParser, ManyEmptyLines)
{
	std::vector<std::string> yamlLines = {
		"key1: value1",
		"",
		"",
		"key2: value2",
		"",
		"key3: value3",
	};
	MappingNode root("");
	Logger logger;
	EXPECT_TRUE(parse(yamlLines, root, logger));
	EXPECT_EQ(root.getNodes().size(), 3);
	if (root.getNodes().size() != 3)
		return;
	EXPECT_EQ(root.getNodes().at(0)->getKey(), "key1");
	EXPECT_EQ(root.getNodes().at(1)->getKey(), "key2");
	EXPECT_EQ(root.getNodes().at(2)->getKey(), "key3");
}

TEST(YamlParser, DeepNesting)
{
	std::vector<std::string> yamlLines = {
		"key1:",
		"  key2:",
		"    key3:",
		"      key4:",
		"        key5: value5",
	};
	MappingNode root("");
	Logger logger;
	EXPECT_TRUE(parse(yamlLines, root, logger));
	EXPECT_EQ(root.getNodes().size(), 1);
	if (root.getNodes().size() != 1)
		return;
	const MappingNode *mappingNode1 = dynamic_cast<const MappingNode *>(root.getNodes().at(0));
	EXPECT_NE(mappingNode1, nullptr);
	if (mappingNode1 == nullptr)
		return;
	EXPECT_EQ(mappingNode1->getKey(), "key1");
	EXPECT_EQ(mappingNode1->getNodes().size(), 1);
	if (mappingNode1->getNodes().size() < 1)
		return;
	const MappingNode *mappingNode2 = dynamic_cast<const MappingNode *>(mappingNode1->getNodes().at(0));
	EXPECT_NE(mappingNode2, nullptr);
	if (mappingNode2 == nullptr)
		return;
	EXPECT_EQ(mappingNode2->getKey(), "key2");
	EXPECT_EQ(mappingNode2->getNodes().size(), 1);
	if (mappingNode2->getNodes().size() < 1)
		return;
	const MappingNode *mappingNode3 = dynamic_cast<const MappingNode *>(mappingNode2->getNodes().at(0));
	EXPECT_NE(mappingNode3, nullptr);
	if (mappingNode3 == nullptr)
		return;
	EXPECT_EQ(mappingNode3->getKey(), "key3");
	EXPECT_EQ(mappingNode3->getNodes().size(), 1);
	if (mappingNode3->getNodes().size() < 1)
		return;
	const MappingNode *mappingNode4 = dynamic_cast<const MappingNode *>(mappingNode3->getNodes().at(0));
	EXPECT_NE(mappingNode4, nullptr);
	if (mappingNode4 == nullptr)
		return;
	EXPECT_EQ(mappingNode4->getKey(), "key4");
	EXPECT_EQ(mappingNode4->getNodes().size(), 1);
	if (mappingNode4->getNodes().size() < 1)
		return;
	const ScalarNode *scalarNode5 = dynamic_cast<const ScalarNode *>(mappingNode4->getNodes().at(0));
	EXPECT_NE(scalarNode5, nullptr);
	if (scalarNode5 == nullptr)
		return;
	EXPECT_EQ(scalarNode5->getKey(), "key5");
	EXPECT_EQ(scalarNode5->getValue(), "value5");
}

}	 // namespace yaml

}	 // namespace webserv

#pragma once

#include <string>
#include <yaml/MappingNode.hpp>
#include <yaml/ScalarNode.hpp>

namespace webserv
{

namespace yaml
{

inline const ScalarNode &getScalarNode(const NodeBase &node)
{
	const ScalarNode *ptr = dynamic_cast<const ScalarNode *>(&node);
	if (ptr == NULL)
		throw std::runtime_error("getScalarNode: not a ScalarNode. Key: " + node.getKey());
	return *ptr;
}

inline const MappingNode &getMappingNode(const NodeBase &node)
{
	const MappingNode *ptr = dynamic_cast<const MappingNode *>(&node);
	if (ptr == NULL)
		throw std::runtime_error("getMappingNode: not a MappingNode. Key: " + node.getKey());
	return *ptr;
}

inline const ScalarNode &getScalarNode(const MappingNode &node, const std::string &key)
{
	return getScalarNode(node.get(key));
}

inline const MappingNode &getMappingNode(const MappingNode &node, const std::string &key)
{
	return getMappingNode(node.get(key));
}

}	 // namespace yaml

}	 // namespace webserv

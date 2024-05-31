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
	return dynamic_cast<const ScalarNode &>(node);
}

inline const MappingNode &getMappingNode(const NodeBase &node)
{
	return dynamic_cast<const MappingNode &>(node);
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

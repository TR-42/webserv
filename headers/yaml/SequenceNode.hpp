#pragma once

#include <vector>

#include "yaml/NodeBase.hpp"

namespace webserv
{

namespace yaml
{

class SequenceNode : public NodeBase
{
 private:
	std::vector<NodeBase *> _nodes;

 public:
	SequenceNode(const std::string &key) : NodeBase(key) {}
	SequenceNode(const SequenceNode &other) : NodeBase(other)
	{
		for (std::vector<NodeBase *>::const_iterator it = other._nodes.begin(); it != other._nodes.end(); ++it)
			this->_nodes.push_back((*it)->clone());
	}
	~SequenceNode()
	{
		for (std::vector<NodeBase *>::iterator it = this->_nodes.begin(); it != this->_nodes.end(); ++it)
			delete *it;
	}
	void addNode(const NodeBase &node) { this->_nodes.push_back(node.clone()); }
	const std::vector<NodeBase *> &getNodes() const { return this->_nodes; }
	SequenceNode &operator=(const SequenceNode &other)
	{
		if (this == &other)
			return *this;
		NodeBase::operator=(other);
		for (std::vector<NodeBase *>::iterator it = this->_nodes.begin(); it != this->_nodes.end(); ++it)
			delete *it;
		this->_nodes.clear();
		for (std::vector<NodeBase *>::const_iterator it = other._nodes.begin(); it != other._nodes.end(); ++it)
			this->_nodes.push_back((*it)->clone());

		return *this;
	}

	NodeBase *clone() const { return new SequenceNode(*this); }
};

}	 // namespace yaml

}	 // namespace webserv

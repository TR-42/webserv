#pragma once

#include <stdexcept>
#include <vector>
#include <yaml/NodeBase.hpp>

namespace webserv
{

namespace yaml
{

typedef std::vector<NodePtr> NodeVector;

class MappingNode : public NodeBase
{
 private:
	NodeVector _nodes;

 public:
	MappingNode(const std::string &key) : NodeBase(key) {}
	MappingNode(const MappingNode &other) : NodeBase(other)
	{
		for (NodeVector::const_iterator it = other._nodes.begin(); it != other._nodes.end(); ++it)
			this->_nodes.push_back((*it)->clone());
	}
	virtual ~MappingNode()
	{
		for (NodeVector::iterator it = this->_nodes.begin(); it != this->_nodes.end(); ++it)
			delete *it;
	}
	void addNode(const NodeBase &node)
	{
		this->_nodes.push_back(node.clone());
	}
	const NodeVector &getNodes() const { return this->_nodes; }
	bool has(const std::string &key) const
	{
		for (NodeVector::const_iterator it = this->_nodes.begin(); it != this->_nodes.end(); ++it)
			if ((*it)->getKey() == key)
				return true;
		return false;
	}
	const NodeBase &get(const std::string &key) const
	{
		for (NodeVector::const_iterator it = this->_nodes.begin(); it != this->_nodes.end(); ++it)
			if ((*it)->getKey() == key)
				return **it;
		throw std::runtime_error("MappingNode: key not found");
	}

	MappingNode &operator=(const MappingNode &other)
	{
		if (this == &other)
			return *this;
		NodeBase::operator=(other);
		for (NodeVector::iterator it = this->_nodes.begin(); it != this->_nodes.end(); ++it)
			delete *it;
		this->_nodes.clear();
		for (NodeVector::const_iterator it = other._nodes.begin(); it != other._nodes.end(); ++it)
			this->_nodes.push_back((*it)->clone());

		return *this;
	}

	NodePtr clone() const { return new MappingNode(*this); }

	inline bool equals(const MappingNode &other) const
	{
		if (this->_nodes.size() != other._nodes.size())
			return false;
		for (size_t i = 0; i < this->_nodes.size(); ++i) {
			if (*this->_nodes[i] != *other._nodes[i])
				return false;
		}
		return true;
	}

	inline virtual bool equals(const NodeBase &other) const
	{
		const MappingNode *other_mapping = dynamic_cast<const MappingNode *>(&other);
		if (other_mapping == NULL)
			return false;
		return this->equals(*other_mapping);
	}
};

}	 // namespace yaml

}	 // namespace webserv

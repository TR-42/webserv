#pragma once
#include <string>

namespace webserv
{

namespace yaml
{

class NodeBase;
typedef const NodeBase *NodePtr;

class NodeBase
{
 private:
	std::string _key;

 public:
	NodeBase(const std::string &key) : _key(key) {}
	const std::string &getKey() const { return this->_key; }

	NodeBase(const NodeBase &other) : _key(other.getKey()) {}

	NodeBase &operator=(const NodeBase &other)
	{
		if (this == &other)
			return *this;
		this->_key = other._key;
		return *this;
	}

	virtual NodePtr clone() const = 0;
};

}	 // namespace yaml

}	 // namespace webserv

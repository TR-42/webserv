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
	virtual ~NodeBase() {}
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
	virtual bool equals(const NodeBase &other) const { return this->_key == other._key; }
	friend bool operator==(const NodeBase &lhs, const NodeBase &rhs) { return lhs.equals(rhs); }
	friend bool operator!=(const NodeBase &lhs, const NodeBase &rhs) { return !lhs.equals(rhs); }
};

}	 // namespace yaml

}	 // namespace webserv

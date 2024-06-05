#pragma once

#include <string>
#include <yaml/NodeBase.hpp>

namespace webserv
{

namespace yaml
{

class ScalarNode : public NodeBase
{
 private:
	std::string _value;

 public:
	ScalarNode(const std::string &key, const std::string &value) : NodeBase(key), _value(value) {}
	const std::string &getValue() const { return this->_value; }
	ScalarNode(const ScalarNode &other) : NodeBase(other), _value(other._value) {}
	ScalarNode &operator=(const ScalarNode &other)
	{
		if (this == &other)
			return *this;
		NodeBase::operator=(other);
		this->_value = other._value;

		return *this;
	}

	const NodeBase *clone() const { return new ScalarNode(*this); }

	inline bool equals(const ScalarNode &other) const
	{
		return this->_value == other._value;
	}

	inline virtual bool equals(const NodeBase &other) const
	{
		const ScalarNode *other_scalar = dynamic_cast<const ScalarNode *>(&other);
		if (other_scalar == NULL)
			return false;
		return this->equals(*other_scalar);
	}
};

}	 // namespace yaml

}	 // namespace webserv

#pragma once

#include <map>
#include <string>
#include <vector>

namespace webserv
{

class HttpFieldMap
{
 private:
	typedef std::map<std::string, std::vector<std::string> > FieldMapType;
	FieldMapType fieldMap;

 public:
	std::vector<std::string> getValueList(const std::string &name) const;
	void addValue(const std::string &name, const std::string &value);

	bool isNameExists(const std::string &name) const;

	void appendToVector(std::vector<uint8_t> &dst) const;

	bool empty() const;
};

}	 // namespace webserv

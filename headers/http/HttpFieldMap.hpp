#pragma once

#include <algorithm>
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

	// Helper function to convert a string to uppercase
	std::string toUpper(const std::string &str) const;

 public:
	std::vector<std::string> getValueList(const std::string &name) const;
	void addValue(const std::string &name, const std::string &value);

	bool isNameExists(const std::string &name) const;

	void appendToVector(std::vector<uint8_t> &dst) const;

	bool empty() const;
};

}	 // namespace webserv

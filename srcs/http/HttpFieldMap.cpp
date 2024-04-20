#include "http/HttpFieldMap.hpp"

#include <cctype>

namespace webserv
{

static std::string capitalize(const std::string &str)
{
	std::string upperStr = str;
	for (size_t i = 1; i < upperStr.size(); i++) {
		if (std::islower(upperStr[i]) && !std::isalnum(upperStr[i - 1])) {
			upperStr[i] = upperStr[i] - 'a' + 'A';
		}
	}
	return upperStr;
}

std::vector<std::string> HttpFieldMap::getValueList(const std::string &name) const
{
	std::string upperName = capitalize(name);
	FieldMapType::const_iterator it = this->fieldMap.find(upperName);
	if (it == this->fieldMap.end()) {
		return std::vector<std::string>();
	}
	return it->second;
}
void HttpFieldMap::addValue(const std::string &name, const std::string &value)
{
	std::string upperName = capitalize(name);
	fieldMap[upperName].push_back(value);
}

bool HttpFieldMap::isNameExists(const std::string &name) const
{
	std::string upperName = capitalize(name);
	return fieldMap.find(upperName) != fieldMap.end();
}

void HttpFieldMap::appendToVector(std::vector<uint8_t> &dst) const
{
	for (
		FieldMapType::const_iterator it = fieldMap.begin();
		it != fieldMap.end();
		++it
	) {
		// TODO: 同じ名前のフィールドが複数ある場合の処理
		for (
			std::vector<std::string>::const_iterator it2 = it->second.begin();
			it2 != it->second.end();
			++it2
		) {
			std::string fieldLine = it->first + ": " + *it2 + "\r\n";
			dst.insert(dst.end(), fieldLine.begin(), fieldLine.end());
		}
	}
}

bool HttpFieldMap::empty() const
{
	return fieldMap.empty();
}

}	 // namespace webserv

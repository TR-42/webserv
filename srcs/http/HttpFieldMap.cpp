#include "http/HttpFieldMap.hpp"

namespace webserv
{

// Helper function to convert a string to uppercase
std::string HttpFieldMap::toUpper(const std::string &str) const
{
	std::string upperStr = str;
	// 最初の文字だけ大文字にする
	// std::transform(upperStr.begin(), upperStr.end(), upperStr.begin(), ::toupper);

	return upperStr;
}

std::vector<std::string> HttpFieldMap::getValueList(const std::string &name) const
{
	std::string upperName = toUpper(name);
	FieldMapType::const_iterator it = this->fieldMap.find(upperName);
	if (it == this->fieldMap.end()) {
		return std::vector<std::string>();
	}
	return it->second;
}
void HttpFieldMap::addValue(const std::string &name, const std::string &value)
{
	std::string upperName = toUpper(name);
	fieldMap[upperName].push_back(value);
}

bool HttpFieldMap::isNameExists(const std::string &name) const
{
	std::string upperName = toUpper(name);
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

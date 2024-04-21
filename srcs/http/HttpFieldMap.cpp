#include "http/HttpFieldMap.hpp"

#include <cctype>
#include <utils.hpp>

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

static void _appendToVector(
	const std::string &name,
	const std::string &value,
	std::vector<uint8_t> &dst
)
{
	std::string fieldLine = name + ": " + value + "\r\n";
	dst.insert(dst.end(), fieldLine.begin(), fieldLine.end());
}
void HttpFieldMap::appendToVector(
	std::vector<uint8_t> &dst,
	std::vector<uint8_t> body
) const
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
			_appendToVector(it->first, *it2, dst);
		}
	}

	if (!this->isNameExists("Content-Length") && !body.empty()) {
		_appendToVector("Content-Length", utils::to_string(body.size()), dst);
	}
	if (!this->isNameExists(("Date"))) {
		_appendToVector("Date", utils::getHttpTimeStr(), dst);
	}
}

bool HttpFieldMap::empty() const
{
	return fieldMap.empty();
}

bool webserv::HttpFieldMap::tryGetContentLength(
	size_t &contentLength
) const
{
	if (!this->isNameExists("Content-Length")) {
		return false;
	}

	std::vector<std::string> valueList = this->getValueList("Content-Length");
	if (valueList.empty()) {
		return false;
	}
	utils::stoul(valueList[0], contentLength);
	return true;
}

}	 // namespace webserv

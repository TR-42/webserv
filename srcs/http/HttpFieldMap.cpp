#include "http/HttpFieldMap.hpp"

#include <cctype>
#include <utils/getTimeStr.hpp>
#include <utils/stoul.hpp>
#include <utils/to_string.hpp>

namespace webserv
{

static std::string capitalize(const std::string &str)
{
	if (str.empty()) {
		return str;
	}

	std::string upperStr = str;

	if (std::islower(upperStr[0])) {
		upperStr[0] = std::toupper(upperStr[0]);
	}

	for (size_t i = 1; i < upperStr.size(); i++) {
		if (std::isalpha(str[i])) {
			bool isPrevCharAlnum = std::isalnum(upperStr[i - 1]);
			if (std::isupper(upperStr[i]) && isPrevCharAlnum) {
				upperStr[i] = std::tolower(str[i]);
			} else if (std::islower(upperStr[i]) && !isPrevCharAlnum) {
				upperStr[i] = std::toupper(str[i]);
			}
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

bool operator==(
	const HttpFieldMap &lhs,
	const HttpFieldMap &rhs
)
{
	if (lhs.fieldMap.size() != rhs.fieldMap.size()) {
		return false;
	}

	for (
		HttpFieldMap::FieldMapType::const_iterator it = lhs.fieldMap.begin();
		it != lhs.fieldMap.end();
		++it
	) {
		if (rhs.fieldMap.find(it->first) == rhs.fieldMap.end()) {
			return false;
		}

		if (it->second != rhs.fieldMap.at(it->first)) {
			return false;
		}
	}

	return true;
}

}	 // namespace webserv

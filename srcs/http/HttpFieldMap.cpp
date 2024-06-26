#include "http/HttpFieldMap.hpp"

#include <cctype>
#include <http/exception/BadRequest.hpp>
#include <http/exception/NotImplemented.hpp>
#include <utils/getTimeStr.hpp>
#include <utils/stoul.hpp>
#include <utils/to_string.hpp>

namespace webserv
{

HttpFieldMap::HttpFieldMap()
{
}

HttpFieldMap::HttpFieldMap(
	const HttpFieldMap &src
) : fieldMap(src.fieldMap)
{
}

HttpFieldMap &HttpFieldMap::operator=(
	const HttpFieldMap &src
)
{
	if (this == &src) {
		return *this;
	}

	this->fieldMap = src.fieldMap;

	return *this;
}

HttpFieldMap::~HttpFieldMap()
{
}

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

void HttpFieldMap::removeKey(const std::string &name)
{
	// 存在しないキーを削除しても問題ない
	fieldMap.erase(capitalize(name));
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
	if (this->isNameExists("Location")) {
		_appendToVector("Location", fieldMap.find("Location")->second[0], dst);
	}

	for (
		FieldMapType::const_iterator it = fieldMap.begin();
		it != fieldMap.end();
		++it
	) {
		if (it->first == "Location" || it->first == "Transfer-Encoding" || it->first == "Connection") {
			continue;
		}

		// 同じ名前のフィールドが複数あるのはSet-Cookieだけ
		for (
			std::vector<std::string>::const_iterator it2 = it->second.begin();
			it2 != it->second.end();
			++it2
		) {
			_appendToVector(it->first, *it2, dst);
		}
	}

	// 長さが0でもContent-Lengthを追加する
	if (!this->isNameExists("Content-Length")) {
		_appendToVector("Content-Length", utils::to_string(body.size()), dst);
	}
	if (!this->isNameExists(("Date"))) {
		_appendToVector("Date", utils::getHttpTimeStr(), dst);
	}

	_appendToVector("Connection", "close", dst);
}

bool HttpFieldMap::empty() const
{
	return fieldMap.empty();
}

HttpFieldMap::FieldMapType::const_iterator HttpFieldMap::cbegin() const
{
	return this->fieldMap.begin();
}
HttpFieldMap::FieldMapType::const_iterator HttpFieldMap::cend() const
{
	return this->fieldMap.end();
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
	if (!utils::stoul(valueList[0], contentLength)) {
		throw http::exception::BadRequest("Content-Length: " + valueList[0]);
	}

	return true;
}

bool HttpFieldMap::isChunked() const
{
	if (!this->isNameExists("Transfer-Encoding")) {
		return false;
	}

	std::vector<std::string> valueList = this->getValueList("Transfer-Encoding");
	if (valueList.empty()) {
		return false;
	}

	if (valueList[0] != "chunked") {
		throw http::exception::NotImplemented("Transfer-Encoding: " + valueList[0]);
	}

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

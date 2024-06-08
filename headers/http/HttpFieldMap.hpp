#pragma once

#include <map>
#include <string>
#include <types.hpp>
#include <vector>

namespace webserv
{

class HttpFieldMap
{
 public:
	typedef std::map<std::string, std::vector<std::string> > FieldMapType;

 private:
	FieldMapType fieldMap;

 public:
	HttpFieldMap();
	HttpFieldMap(const HttpFieldMap &src);
	HttpFieldMap &operator=(const HttpFieldMap &src);
	virtual ~HttpFieldMap();

	std::vector<std::string> getValueList(const std::string &name) const;
	void addValue(const std::string &name, const std::string &value);

	bool isNameExists(const std::string &name) const;

	void appendToVector(
		std::vector<uint8_t> &dst,
		std::vector<uint8_t> body = std::vector<uint8_t>()
	) const;

	bool empty() const;

	FieldMapType::const_iterator cbegin() const;
	FieldMapType::const_iterator cend() const;

	/**
	 * @brief FieldLineとしてContentLengthがあれば取得し、contentLengthに格納する
	 *
	 * @param contentLength ContentLengthの値を格納する変数
	 * @return true ContentLengthが設定されていた (parseエラーの場合も含む)
	 * @return false ContentLengthが設定されていなかった (見つからなかった)
	 */
	bool tryGetContentLength(size_t &contentLength) const;

	bool isChunked() const;

	friend bool operator==(const HttpFieldMap &lhs, const HttpFieldMap &rhs);
};

}	 // namespace webserv

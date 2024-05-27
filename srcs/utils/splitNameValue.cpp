#include <cstring>
#include <utils/splitNameValue.hpp>
#include <utils/strtrim.hpp>

namespace webserv
{

namespace utils
{

std::pair<std::string, std::string> splitNameValue(
	const std::vector<uint8_t> &data
)
{
	const uint8_t *requestRawData = data.data();
	size_t newlinePos = data.size();
	const uint8_t *separatorPos = (const uint8_t *)std::memchr(requestRawData, ':', newlinePos);
	if (separatorPos == NULL) {
		return std::pair<std::string, std::string>();
	}
	size_t lenToSeparatorPos = separatorPos - requestRawData;
	if (lenToSeparatorPos == 0) {
		return std::pair<std::string, std::string>();
	}
	std::string _Key = std::string((const char *)requestRawData, lenToSeparatorPos);
	size_t keyLen = _Key.size();
	if (std::isspace(_Key[keyLen - 1])) {
		return std::pair<std::string, std::string>();
	}
	std::string _Value = utils::strtrim(std::string((const char *)separatorPos + 1, newlinePos - lenToSeparatorPos - 1));
	return std::pair<std::string, std::string>(_Key, _Value);
}

std::pair<std::string, std::string> splitNameValue(
	const std::string &data
)
{
	size_t newlinePos = data.size();
	size_t separatorPos = data.find(':');
	if (separatorPos == std::string::npos) {
		return std::pair<std::string, std::string>();
	}
	size_t lenToSeparatorPos = separatorPos;
	if (lenToSeparatorPos == 0) {
		return std::pair<std::string, std::string>();
	}
	std::string _Key = data.substr(0, lenToSeparatorPos);
	size_t keyLen = _Key.size();
	if (std::isspace(_Key[keyLen - 1])) {
		return std::pair<std::string, std::string>();
	}
	std::string _Value = utils::strtrim(data.substr(separatorPos + 1, newlinePos - lenToSeparatorPos - 1));
	return std::pair<std::string, std::string>(_Key, _Value);
}

}	 // namespace utils

}	 // namespace webserv

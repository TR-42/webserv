#pragma once

#include <sys/socket.h>

#include <cstring>
#include <ctime>
#include <poll/PollEventResult.hpp>
#include <string>
#include <vector>

namespace webserv
{

namespace utils
{

std::string strtrim(
	const std::string &str
);

/**
 * @brief パーセントエンコードされた文字列をデコードする
 *
 * @param str デコードする文字列
 * @return std::string デコードされた文字列
 */
std::string url_decode(
	const std::string &str
);

bool stoul(
	const std::string &str,
	unsigned long &result
);

std::string to_string(
	int i
);

std::string to_string(
	size_t i
);

std::string to_string(
	const struct sockaddr &addr
);
std::string to_string(
	const struct sockaddr_in &addr
);
std::string to_string(
	const struct sockaddr_in6 &addr
);

std::string to_string(
	const PollEventResultType &result
);

std::string getHttpTimeStr();
std::string getHttpTimeStr(
	time_t time
);

std::string getIso8601ShortTimeStr();
std::string getIso8601ShortTimeStr(
	time_t time
);

std::string modeToString(
	mode_t mode
);

std::string normalizePath(
	const std::string &path
);

std::vector<uint8_t> *pickLine(
	std::vector<uint8_t> &unpickedData
);

std::pair<std::string, std::string> splitNameValue(
	const std::vector<uint8_t> &data
);

bool strcasecmp(
	const std::string &str1,
	const std::string &str2
);

std::vector<std::string> splitWithSpace(
	const std::string &str,
	const size_t maxCount
);

}	 // namespace utils

}	 // namespace webserv

#pragma once

#include <sys/socket.h>

#include <ctime>
#include <poll/PollEventResult.hpp>
#include <string>

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

std::string modeToString(
	mode_t mode
);

std::string normalizePath(
	const std::string &path
);

}	 // namespace utils

}	 // namespace webserv

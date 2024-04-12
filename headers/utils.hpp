#pragma once

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

}	 // namespace utils

}	 // namespace webserv

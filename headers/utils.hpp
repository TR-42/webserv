#pragma once

#include <string>

namespace webserv
{

namespace utils
{

/**
 * @brief パーセントエンコードされた文字列をデコードする
 *
 * @param str デコードする文字列
 * @return std::string デコードされた文字列
 */
std::string url_decode(
		const std::string &str
);

}	 // namespace utils

}	 // namespace webserv

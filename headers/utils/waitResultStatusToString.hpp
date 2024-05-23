#pragma once

#include <string>

namespace webserv
{

namespace utils
{

/**
 * @brief waitpid() の結果を文字列に変換する
 *
 * @param result waitpid() の結果
 * @return std::string waitpid() の結果を文字列に変換したもの
 */
std::string waitResultStatusToString(int result);

}	 // namespace utils

}	 // namespace webserv

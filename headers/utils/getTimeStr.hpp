#pragma once

#include <ctime>
#include <string>

namespace webserv
{

namespace utils
{

std::string getHttpTimeStr();
std::string getHttpTimeStr(
	time_t time
);

std::string getIso8601ShortTimeStr();
std::string getIso8601ShortTimeStr(
	time_t time
);

}	 // namespace utils

}	 // namespace webserv

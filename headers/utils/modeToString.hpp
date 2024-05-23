#pragma once

#include <sys/stat.h>

#include <string>

namespace webserv
{

namespace utils
{

std::string modeToString(
	mode_t mode
);

}	 // namespace utils

}	 // namespace webserv

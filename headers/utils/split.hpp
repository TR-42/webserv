#pragma once

#include <string>
#include <vector>

namespace webserv
{

namespace utils
{

std::vector<std::string> split(
	const std::string &str,
	const std::string &delim
);

std::vector<std::string> split(
	const std::string &str,
	char delim
);

}	 // namespace utils

}	 // namespace webserv

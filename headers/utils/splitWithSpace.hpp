#pragma once

#include <string>
#include <vector>

namespace webserv
{

namespace utils
{

std::vector<std::string> splitWithSpace(
	const std::string &str,
	const size_t maxCount
);

}	 // namespace utils

}	 // namespace webserv

#pragma once

#include <string>
#include <types.hpp>
#include <vector>

namespace webserv
{

namespace utils
{

std::pair<std::string, std::string> splitNameValue(
	const std::vector<uint8_t> &data
);

std::pair<std::string, std::string> splitNameValue(
	const std::string &data
);

}	 // namespace utils

}	 // namespace webserv

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

}	 // namespace utils

}	 // namespace webserv

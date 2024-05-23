#pragma once

#include <cstdint>
#include <string>
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

#pragma once

#include <types.hpp>
#include <vector>

namespace webserv
{

namespace utils
{

std::vector<uint8_t> *pickLine(
	std::vector<uint8_t> &unpickedData
);

}	 // namespace utils

}	 // namespace webserv

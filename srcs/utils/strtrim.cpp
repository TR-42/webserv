#include "utils.hpp"

std::string webserv::utils::strtrim(
	const std::string &str
)
{
	size_t begin = 0;
	size_t end = str.size();
	while (begin < end && std::isspace(str[begin])) {
		++begin;
	}
	while (begin < end && std::isspace(str[end - 1])) {
		--end;
	}
	return str.substr(begin, end - begin);
}

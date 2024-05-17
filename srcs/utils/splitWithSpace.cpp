#include "utils.hpp"

namespace webserv
{

namespace utils
{

std::vector<std::string> splitWithSpace(const std::string &str, const size_t maxCount)
{
	std::vector<std::string> result;
	size_t start = 0;
	size_t end = 0;
	size_t count = 0;

	while (end < str.size()) {
		while (end < str.size() && str[end] == ' ') {
			++end;
		}
		start = end;
		if (maxCount != 0 && maxCount - 1 <= count) {
			result.push_back(str.substr(end));
			break;
		}
		while (end < str.size() && str[end] != ' ') {
			++end;
		}
		if (start < end) {
			result.push_back(str.substr(start, end - start));
			++count;
		}
	}

	return result;
}

}	 // namespace utils

}	 // namespace webserv

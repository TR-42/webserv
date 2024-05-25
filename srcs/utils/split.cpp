#include <utils/split.hpp>

namespace webserv
{

namespace utils
{

std::vector<std::string> split(
	const std::string &str,
	const std::string &delim
)
{
	std::vector<std::string> result;

	if (str.empty()) {
		return result;
	}
	if (delim.empty()) {
		result.push_back(str);
		return result;
	}

	size_t start = 0;
	while (true) {
		size_t end = str.find(delim, start);
		if (end == std::string::npos) {
			result.push_back(str.substr(start));
			break;
		}
		if (start != end) {
			result.push_back(str.substr(start, end - start));
		}
		start = end + delim.size();
	}

	return result;
}

std::vector<std::string> split(
	const std::string &str,
	char delim
)
{
	std::vector<std::string> result;

	if (str.empty()) {
		return result;
	}
	if (delim == '\0') {
		result.push_back(str);
		return result;
	}

	size_t start = 0;
	while (true) {
		size_t end = str.find(delim, start);
		if (end == std::string::npos) {
			result.push_back(str.substr(start));
			break;
		}
		if (start != end) {
			result.push_back(str.substr(start, end - start));
		}
		start = end + 1;
	}

	return result;
}

}	 // namespace utils

}	 // namespace webserv

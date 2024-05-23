#include <cctype>
#include <utils/strcasecmp.hpp>

namespace webserv
{

namespace utils
{

bool strcasecmp(const std::string &str1, const std::string &str2)
{
	size_t str1Size = str1.size();
	size_t str2Size = str2.size();
	if (str1Size != str2Size) {
		return false;
	}
	for (size_t i = 0; i < str1Size; ++i) {
		if (std::tolower(str1[i]) != std::tolower(str2[i])) {
			return false;
		}
	}
	return true;
}

}	 // namespace utils

}	 // namespace webserv

#include "math/math.hpp"
#include "utils.hpp"

namespace webserv
{

namespace utils
{

bool stoul(const std::string &str, unsigned long &result)
{
	size_t strSize = str.size();
	if (strSize == 0) {
		return false;
	}
	for (size_t i = 0; i < strSize; ++i) {
		if (str[i] < '0' || '9' < str[i]) {
			return false;
		}
	}
	result = 0;
	for (size_t i = 0; i < strSize; ++i) {
		if (!webserv::math::can_mul(result, 10ul)) {
			return false;
		}
		result *= 10;
		unsigned long digit = str[i] - '0';
		if (!webserv::math::can_add(result, digit)) {
			return false;
		}
		result += digit;
	}
	return true;
}

}	 // namespace utils

}	 // namespace webserv

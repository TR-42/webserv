#include <cstdio>
#include <string>

namespace webserv
{

namespace utils
{

std::string to_string(int i)
{
	char buf[16];
	std::snprintf(buf, sizeof(buf), "%d", i);
	return std::string(buf);
}

}	 // namespace utils

}	 // namespace webserv

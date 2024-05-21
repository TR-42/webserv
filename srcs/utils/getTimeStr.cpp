#include <utils.hpp>

namespace webserv
{

namespace utils
{

std::string getHttpTimeStr()
{
	return getHttpTimeStr(std::time(NULL));
}

std::string getHttpTimeStr(
	time_t time
)
{
	char dateStr[64];
	struct tm *now_tm = std::gmtime(&time);

	std::strftime(dateStr, sizeof(dateStr), "%a, %d %b %Y %H:%M:%S GMT", now_tm);

	return dateStr;
}

std::string getIso8601ShortTimeStr()
{
	return getIso8601ShortTimeStr(std::time(NULL));
}
std::string getIso8601ShortTimeStr(
	time_t time
)
{
	char dateStr[64];
	struct tm *now_tm = std::gmtime(&time);

	std::strftime(dateStr, sizeof(dateStr), "%Y%m%dT%H%M%SZ", now_tm);

	return dateStr;
}

}	 // namespace utils

}	 // namespace webserv

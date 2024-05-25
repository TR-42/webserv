#include <utils/getTimeStr.hpp>

#ifdef GTEST
time_t __webserv_timeMockValue = 0;
#define GET_CURRENT_TIME() (__webserv_timeMockValue)
#else
#define GET_CURRENT_TIME() (std::time(NULL))
#endif

namespace webserv
{

namespace utils
{

std::string getHttpTimeStr()
{
	return getHttpTimeStr(GET_CURRENT_TIME());
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
	return getIso8601ShortTimeStr(GET_CURRENT_TIME());
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

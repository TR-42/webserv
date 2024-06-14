#pragma once

#include <sys/socket.h>

#include <cgi/CgiResponseMode.hpp>
#include <poll/PollEventResult.hpp>
#include <string>

namespace webserv
{

namespace utils
{

std::string to_string(
	int i
);

std::string to_string(
	size_t i
);

std::string to_string(
	const struct sockaddr &addr
);
std::string to_string(
	const struct sockaddr_in &addr
);
std::string to_string(
	const struct sockaddr_in6 &addr
);

std::string to_string(
	const PollEventResultType &result
);

std::string to_string(
	const CgiResponseModeType &mode
);

}	 // namespace utils

}	 // namespace webserv

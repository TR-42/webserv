#include <arpa/inet.h>

#include <iostream>
#include <sstream>
#include <string>
#include <utils/to_string.hpp>

namespace webserv
{

namespace utils
{

std::string to_string(int i)
{
	std::stringstream ss;
	ss << std::dec << i;
	return ss.str();
}

std::string to_string(size_t i)
{
	std::stringstream ss;
	ss << std::dec << i;
	return ss.str();
}

static std::string _ipv4_to_string(
	const struct in_addr &addr
)
{
	std::stringstream ss;

	uint32_t addr32 = ntohl(addr.s_addr);
	ss << std::dec
		 << (addr32 >> (3 * 8))
		 << '.'
		 << ((addr32 >> (2 * 8)) & 0xff)
		 << '.'
		 << ((addr32 >> (1 * 8)) & 0xff)
		 << '.'
		 << ((addr32 >> (0 * 8)) & 0xff);

	return ss.str();
}

static std::string _ipv6_to_string(
	const struct in6_addr &addr
)
{
	std::stringstream ss;

	for (size_t i = 0; i < 8; i++) {
		uint16_t part = ntohs(*((uint16_t *)(&addr.s6_addr[i * 2])));
		ss << std::hex << part;
		if (i < 7) {
			ss << ':';
		}
	}

	return ss.str();
}

std::string to_string(
	const struct sockaddr &addr
)
{
	if (addr.sa_family == AF_INET) {
		return to_string(reinterpret_cast<const struct sockaddr_in &>(addr));
	} else if (addr.sa_family == AF_INET6) {
		return to_string(reinterpret_cast<const struct sockaddr_in6 &>(addr));
	} else {
		return "(unknown address)";
	}
}
std::string to_string(
	const sockaddr_in &addr
)
{
	return _ipv4_to_string(addr.sin_addr);
}
std::string to_string(
	const sockaddr_in6 &addr
)
{
	return _ipv6_to_string(addr.sin6_addr);
}

std::string to_string(
	const PollEventResultType &result
)
{
	switch (result) {
		case PollEventResult::OK:
			return "PollEventResult::OK";
		case PollEventResult::DISPOSE_REQUEST:
			return "PollEventResult::DISPOSE_REQUEST";
		case PollEventResult::ERROR:
			return "PollEventResult::ERROR";
		default:
			return "PollEventResult::(unknown result)";
	}
}

std::string to_string(
	const CgiResponseModeType &mode
)
{
	switch (mode) {
		case CgiResponseMode::DOCUMENT:
			return "CgiResponseMode::DOCUMENT";
		case CgiResponseMode::LOCAL_REDIRECT:
			return "CgiResponseMode::LOCAL_REDIRECT";
		case CgiResponseMode::CLIENT_REDIRECT:
			return "CgiResponseMode::CLIENT_REDIRECT";
		case CgiResponseMode::CLIENT_REDIRECT_WITH_DOCUMENT:
			return "CgiResponseMode::CLIENT_REDIRECT_WITH_DOCUMENT";
		default:
			return "CgiResponseMode::(unknown mode)";
	}
}

}	 // namespace utils

}	 // namespace webserv

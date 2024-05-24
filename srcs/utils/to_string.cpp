#include <arpa/inet.h>

#include <cstdio>
#include <string>
#include <utils/to_string.hpp>

namespace webserv
{

namespace utils
{

std::string to_string(int i)
{
	char buf[16];
	std::sprintf(buf, "%d", i);
	return std::string(buf);
}

std::string to_string(size_t i)
{
	char buf[32];
	std::sprintf(buf, "%zu", i);
	return std::string(buf);
}

static std::string _ipv4_to_string(
	const struct in_addr &addr
)
{
	char buf[16];

	uint32_t addr32 = ntohl(addr.s_addr);
	std::sprintf(
		buf,
		"%d.%d.%d.%d",
		(addr32 >> (3 * 8)) & 0xff,
		(addr32 >> (2 * 8)) & 0xff,
		(addr32 >> (1 * 8)) & 0xff,
		(addr32 >> (0 * 8)) & 0xff
	);

	return std::string(buf);
}

static std::string _ipv6_to_string(
	const struct in6_addr &addr
)
{
	char buf[41];
	char *bufPtr = buf;

	for (size_t i = 0; i < 8; i++) {
		uint16_t part = ntohs(*((uint16_t *)(&addr.s6_addr[i * 2])));
		int len = std::sprintf(
			bufPtr,
			"%x:",
			part
		);
		if (len <= 0) {
			return "(error)";
		}
		bufPtr += len;
	}
	(bufPtr - 1)[0] = '\0';

	return std::string(buf);
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

}	 // namespace utils

}	 // namespace webserv

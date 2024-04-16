#include <arpa/inet.h>

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

static std::string _ipv4_to_string(
	const struct in_addr &addr
)
{
	char buf[15];

	uint32_t addr32 = ntohl(addr.s_addr);
	std::snprintf(
		buf,
		sizeof(buf),
		"%d.%d.%d.%d",
		(addr32 >> (0 * 8)) & 0xff,
		(addr32 >> (1 * 8)) & 0xff,
		(addr32 >> (2 * 8)) & 0xff,
		(addr32 >> (3 * 8)) & 0xff
	);

	return std::string(buf);
}

static std::string _ipv6_to_string(
	const struct in6_addr &addr
)
{
	char buf[41];
	size_t i = 0;

	for (i = 0; i < 8; i++) {
		uint16_t part = ntohs(*((uint16_t *)(&addr.s6_addr[i * 2])));
		std::snprintf(
			buf + i * 5,
			4,
			"%x:",
			part
		);
	}
	buf[40] = '\0';

	return std::string(buf);
}

std::string to_string(
	const struct sockaddr &addr
)
{
	if (addr.sa_family == AF_INET) {
		const struct sockaddr_in &addr4 = reinterpret_cast<const struct sockaddr_in &>(addr);
		return _ipv4_to_string(addr4.sin_addr);
	} else if (addr.sa_family == AF_INET6) {
		const struct sockaddr_in6 &addr6 = reinterpret_cast<const struct sockaddr_in6 &>(addr);
		return _ipv6_to_string(addr6.sin6_addr);
	} else {
		return std::string("(unknown address)");
	}
}

}	 // namespace utils

}	 // namespace webserv

#include <time.h>

#include <cstdlib>
#include <utils/UUIDv7.hpp>

namespace webserv
{

namespace utils
{

#if RAND_MAX < 0xFFFF
#error `RAND_MAX` must be at least 0xFFFF
#endif

UUIDv7::UUIDv7()
{
	struct timespec ts;
	const int ts_result = clock_gettime(CLOCK_REALTIME, &ts);
	if (ts_result == -1) {
		throw std::runtime_error("Failed to get current time");
	}

	this->fromValue(
		ts.tv_sec * 1000 + ts.tv_nsec / 1000000,
		std::rand() & 0xFFFF,
		(std::rand() << 16) | (std::rand() & 0xFFFF),
		(std::rand() << 16) | (std::rand() & 0xFFFF)
	);
}

UUIDv7::UUIDv7(
	const long long unix_ts_ms,
	const uint16_t randA,
	const uint32_t randB_High,
	const uint32_t randB_Low
)
{
	this->fromValue(
		unix_ts_ms,
		randA,
		randB_High,
		randB_Low
	);
}

void UUIDv7::fromValue(
	const long long unix_ts_ms,
	const uint16_t randA,
	const uint32_t randB_High,
	const uint32_t randB_Low
)
{
	const uint8_t version = 0x07;
	const uint8_t variant = 0b10;

	for (size_t i = 0; i < 6; i++) {
		this->uuid[i] = (unix_ts_ms >> ((5 - i) * 8)) & 0xFF;
	}

	this->uuid[6] = (version << 4) | ((randA >> 8) & 0x3F);
	this->uuid[7] = randA & 0xFF;
	this->uuid[8] = (variant << 6) | ((randB_High >> 24) & 0x0F);

	for (size_t i = 0; i < 3; i++) {
		this->uuid[9 + i] = (randB_High >> ((2 - i) * 8)) & 0xFF;
	}

	for (size_t i = 0; i < 4; i++) {
		this->uuid[12 + i] = (randB_Low >> ((3 - i) * 8)) & 0xFF;
	}
}

UUIDv7::UUIDv7(
	const UUID &src
) : UUID(src)
{
	if (src.getVersion() != 0x07) {
		throw std::invalid_argument("UUID version must be 0x07");
	}
}

UUIDv7::UUIDv7(
	const std::string &src
) : UUID(src)
{
	if (this->getVersion() != 0x07) {
		throw std::invalid_argument("UUID version must be 0x07");
	}
}

UUIDv7::UUIDv7(
	const UUIDv7 &src
) : UUID(src)
{
}

UUIDv7 &UUIDv7::operator=(
	const UUIDv7 &src
)
{
	UUID::operator=(src);
	return *this;
}

long long UUIDv7::getTimestamp() const
{
	long long unix_ts_ms = 0;

	for (size_t i = 0; i < 6; i++) {
		unix_ts_ms = (unix_ts_ms << 8) | this->uuid[i];
	}

	return unix_ts_ms;
}

}	 // namespace utils

}	 // namespace webserv

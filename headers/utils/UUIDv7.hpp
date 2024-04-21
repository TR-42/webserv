#pragma once

#include "./UUID.hpp"

namespace webserv
{

namespace utils
{

class UUIDv7 : public UUID
{
 private:
	void fromValue(
		const long long unix_ts_ms,
		const uint16_t randA,
		const uint32_t randB_High,
		const uint32_t randB_Low
	);

 public:
	UUIDv7();
	UUIDv7(
		const UUID &src
	);
	UUIDv7(
		const std::string &src
	);
	UUIDv7(
		const UUIDv7 &src
	);
	UUIDv7(
		const long long unix_ts_ms,
		const uint16_t randA,
		const uint32_t randB_High,
		const uint32_t randB_Low
	);

	UUIDv7 &operator=(
		const UUIDv7 &src
	);

	long long getTimestamp() const;
};

}	 // namespace utils

}	 // namespace webserv

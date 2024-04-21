#pragma once

#include <cstring>
#include <iostream>
#include <string>

namespace webserv
{

namespace utils
{

class UUID
{
 protected:
	static const size_t UUID_BYTES_COUNT = 16;
	static const size_t UUID_STRING_SHORT_LENGTH = UUID_BYTES_COUNT * 2;
	static const size_t UUID_STRING_LENGTH = 36;
	static const size_t UUID_HYPHEN_POS_1 = 8;
	static const size_t UUID_HYPHEN_POS_2 = 13;
	static const size_t UUID_HYPHEN_POS_3 = 18;
	static const size_t UUID_HYPHEN_POS_4 = 23;
	uint8_t uuid[UUID_BYTES_COUNT];

	UUID();
	UUID(
		const uint8_t *src[UUID_BYTES_COUNT]
	);

 public:
	UUID(
		const std::string &src
	);
	UUID(
		const UUID &src
	);

	virtual ~UUID();
	UUID &operator=(
		const UUID &src
	);

	virtual std::string toString() const;

	uint8_t getVersion() const;
};

std::ostream &operator<<(
	std::ostream &os,
	const UUID &uuid
);

}	 // namespace utils

}	 // namespace webserv

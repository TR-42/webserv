#include <utils/UUID.hpp>

namespace webserv
{

namespace utils
{

UUID::UUID()
{
	std::memset(this->uuid, 0, UUID_BYTES_COUNT);
	this->_uuidStr = _getUUIDString(this->uuid);
}

UUID::UUID(
	const UUID &src
)
{
	this->operator=(src);
}

UUID &UUID::operator=(
	const UUID &src
)
{
	if (this == &src) {
		return *this;
	}

	std::memcpy(this->uuid, src.uuid, UUID_BYTES_COUNT);
	this->_uuidStr = src._uuidStr;
	return *this;
}

static uint8_t _getValueFromHexChar(
	const char c
)
{
	if ('0' <= c && c <= '9') {
		return c - '0';
	} else if ('A' <= c && c <= 'F') {
		return c - 'A' + 10;
	} else if ('a' <= c && c <= 'f') {
		return c - 'a' + 10;
	} else {
		throw std::invalid_argument("Invalid character");
	}
}

UUID::UUID(
	const std::string &src
)
{
	size_t srcLength = src.length();
	if (srcLength != UUID_STRING_LENGTH && srcLength != UUID_STRING_SHORT_LENGTH) {
		throw std::invalid_argument("Invalid UUID string length");
	}

	bool isShortFormat = srcLength == UUID_STRING_SHORT_LENGTH;
	if (!isShortFormat) {
		if (src[UUID_HYPHEN_POS_1] != '-' || src[UUID_HYPHEN_POS_2] != '-' || src[UUID_HYPHEN_POS_3] != '-' || src[UUID_HYPHEN_POS_4] != '-') {
			throw std::invalid_argument("Invalid UUID string format");
		}
	}

	bool isUpperCase = false;
	bool isLowerCase = false;
	for (size_t i = 0; i < srcLength; i++) {
		if (!isShortFormat && (i == UUID_HYPHEN_POS_1 || i == UUID_HYPHEN_POS_2 || i == UUID_HYPHEN_POS_3 || i == UUID_HYPHEN_POS_4)) {
			continue;
		} else if ('0' <= src[i] && src[i] <= '9') {
			continue;
		} else if ('a' <= src[i] && src[i] <= 'f') {
			isLowerCase = true;
			if (isUpperCase) {
				throw std::invalid_argument("Invalid UUID string format (mixed case)");
			}
		} else if ('A' <= src[i] && src[i] <= 'F') {
			isUpperCase = true;
			if (isLowerCase) {
				throw std::invalid_argument("Invalid UUID string format (mixed case)");
			}
		} else {
			throw std::invalid_argument("Invalid UUID string format (invalid character)");
		}
	}

	for (size_t i = 0; i < UUID_BYTES_COUNT; i++) {
		size_t c1Index = i * 2;
		if (!isShortFormat) {
			if (UUID_HYPHEN_POS_1 <= c1Index) {
				++c1Index;
			}
			if (UUID_HYPHEN_POS_2 <= c1Index) {
				++c1Index;
			}
			if (UUID_HYPHEN_POS_3 <= c1Index) {
				++c1Index;
			}
			if (UUID_HYPHEN_POS_4 <= c1Index) {
				++c1Index;
			}
		}
		const size_t c2Index = c1Index + 1;

		this->uuid[i] = (_getValueFromHexChar(src[c1Index]) << 4) | _getValueFromHexChar(src[c2Index]);
	}

	this->_uuidStr = _getUUIDString(this->uuid);
}

UUID::UUID(
	const uint8_t *src[UUID_BYTES_COUNT]
)
{
	std::memcpy(this->uuid, src, UUID_BYTES_COUNT);
	this->_uuidStr = _getUUIDString(this->uuid);
}

UUID::~UUID()
{
}

std::string UUID::_getUUIDString(
	const uint8_t *src
)
{
	char buffer[UUID_STRING_LENGTH + 1];

	buffer[UUID_HYPHEN_POS_1] = '-';
	buffer[UUID_HYPHEN_POS_2] = '-';
	buffer[UUID_HYPHEN_POS_3] = '-';
	buffer[UUID_HYPHEN_POS_4] = '-';
	buffer[UUID_STRING_LENGTH] = '\0';
	for (size_t i = 0; i < UUID_BYTES_COUNT; i++) {
		size_t c1Index = i * 2;
		if (UUID_HYPHEN_POS_1 <= c1Index) {
			++c1Index;
		}
		if (UUID_HYPHEN_POS_2 <= c1Index) {
			++c1Index;
		}
		if (UUID_HYPHEN_POS_3 <= c1Index) {
			++c1Index;
		}
		if (UUID_HYPHEN_POS_4 <= c1Index) {
			++c1Index;
		}
		const size_t c2Index = c1Index + 1;

		buffer[c1Index] = "0123456789abcdef"[src[i] >> 4];
		buffer[c2Index] = "0123456789abcdef"[src[i] & 0x0F];
	}

	return std::string(buffer);
}

std::string UUID::toString() const
{
	return this->_uuidStr;
}

std::ostream &operator<<(
	std::ostream &os,
	const UUID &uuid
)
{
	os << uuid.toString();
	return os;
}

uint8_t UUID::getVersion() const
{
	return (this->uuid[6] >> 4) & 0x0F;
}

}	 // namespace utils

}	 // namespace webserv

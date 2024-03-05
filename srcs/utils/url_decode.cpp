#include <iostream>
#include <stdexcept>
#include <utils.hpp>

#define URL_ENCODE_ESCAPE_CHAR '%'
#if URL_ENCODE_ESCAPE_CHAR == '\0'
#error URL_ENCODE_ESCAPE_CHAR must not be '\0'
#endif

static uint8_t _hex_to_uint8(
		const char c
)
{
	if ('0' <= c && c <= '9') {
		return c - '0';
	} else if ('a' <= c && c <= 'f') {
		return c - 'a' + 10;
	} else if ('A' <= c && c <= 'F') {
		return c - 'A' + 10;
	} else {
		throw std::invalid_argument("Invalid hex character");
	}
}

static char _hex_to_char(
		const char upper,
		const char lower
)
{
	return (char)(_hex_to_uint8(upper) << 4 | _hex_to_uint8(lower));
}

std::string webserv::utils::url_decode(
		const std::string &str
)
{
	size_t percent_count = 0;
	const size_t str_len = str.length();

	for (size_t i = 0; i < str_len; i++) {
		if (str[i] == URL_ENCODE_ESCAPE_CHAR) {
			percent_count++;
		}
	}

	char decoded_c_str[str_len - (percent_count * 2) + 1];
	size_t c_str_index = 0;
	size_t str_index = 0;
	size_t percent_index = 0;
	do {
		percent_index = str.find(URL_ENCODE_ESCAPE_CHAR, str_index);
		if (percent_index == std::string::npos) {
			percent_index = str_len;
		}
		size_t copy_len = percent_index - str_index;
		memcpy(decoded_c_str + c_str_index, str.c_str() + str_index, copy_len);
		c_str_index += copy_len;
		if (str[percent_index] == URL_ENCODE_ESCAPE_CHAR) {
			if (str_len <= percent_index + 2) {
				throw std::invalid_argument("Invalid percent encoding");
			}
			decoded_c_str[c_str_index++] = _hex_to_char(str[percent_index + 1], str[percent_index + 2]);
			percent_index += 3;
		}
		str_index = percent_index;
	} while (percent_index < str_len);

	decoded_c_str[str_len - percent_count * 2] = '\0';
	return decoded_c_str;
}

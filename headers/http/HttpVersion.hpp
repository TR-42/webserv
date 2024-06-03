#pragma once

#include <classDefUtils.hpp>
#include <string>
#include <utils/stoul.hpp>
#include <utils/to_string.hpp>

#include "./exception/HttpVersionNotSupported.hpp"

namespace webserv
{

class HttpVersion
{
 private:
	DECL_VAR_GETTER(unsigned long, Major)
	DECL_VAR_GETTER(unsigned long, Minor)

	HttpVersion(
		unsigned long major,
		unsigned long minor
	) : _Major(major),
			_Minor(minor)
	{
	}

 public:
	static inline HttpVersion fromString(const std::string &version)
	{
		std::string::size_type slashPos = version.find('/');
		if (slashPos == std::string::npos) {
			throw http::exception::HttpVersionNotSupported();
		}

		if (version.substr(0, slashPos) != "HTTP") {
			throw http::exception::HttpVersionNotSupported();
		}

		std::string versionStr = version.substr(slashPos + 1);
		std::string::size_type dotPos = versionStr.find('.');
		if (dotPos == std::string::npos) {
			throw http::exception::HttpVersionNotSupported();
		}

		std::string majorStr = versionStr.substr(0, dotPos);
		std::string minorStr = versionStr.substr(dotPos + 1);

		unsigned long major = 0;
		unsigned long minor = 0;
		if (!utils::stoul(majorStr, major) || !utils::stoul(minorStr, minor)) {
			throw http::exception::HttpVersionNotSupported();
		}

		return HttpVersion(major, minor);
	}

	HttpVersion(
		const HttpVersion &src
	) : _Major(src._Major),
			_Minor(src._Minor)
	{
	}

	inline HttpVersion &operator=(
		const HttpVersion &src
	)
	{
		if (this == &src) {
			return *this;
		}

		this->_Major = src._Major;
		this->_Minor = src._Minor;

		return *this;
	}

	~HttpVersion() {}

	inline std::string toString() const
	{
		return "HTTP/" + utils::to_string(this->_Major) + "." + utils::to_string(this->_Minor);
	}

	inline bool operator==(
		const HttpVersion &rhs
	) const
	{
		return this->_Major == rhs._Major && this->_Minor == rhs._Minor;
	}

	inline bool operator!=(
		const HttpVersion &rhs
	) const
	{
		return !(*this == rhs);
	}

	inline bool operator<(
		const HttpVersion &rhs
	) const
	{
		return this->_Major < rhs._Major || (this->_Major == rhs._Major && this->_Minor < rhs._Minor);
	}

	inline bool operator>(
		const HttpVersion &rhs
	) const
	{
		return rhs < *this;
	}

	inline bool operator<=(
		const HttpVersion &rhs
	) const
	{
		return !(*this > rhs);
	}

	inline bool operator>=(
		const HttpVersion &rhs
	) const
	{
		return !(*this < rhs);
	}
};

}	 // namespace webserv

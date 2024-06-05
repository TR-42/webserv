#pragma once

#include <string>
#include <types.hpp>

#include "../classDefUtils.hpp"

#define EQ(name) (lhs._##name == rhs._##name)

namespace webserv
{

class HttpRedirectConfig
{
	DECL_VAR_REF_GETTER_SETTER(std::string, To)
	DECL_VAR_GETTER_SETTER(uint16_t, Code)

 public:
	HttpRedirectConfig();
	HttpRedirectConfig(
		const HttpRedirectConfig &from
	);
	virtual ~HttpRedirectConfig();
	HttpRedirectConfig &operator=(
		const HttpRedirectConfig &from
	);

	HttpRedirectConfig(
		const std::string &to,
		uint16_t code
	);

	friend bool operator==(const HttpRedirectConfig &lhs, const HttpRedirectConfig &rhs)
	{
		return (EQ(To) && EQ(Code));
	}

	friend bool operator!=(const HttpRedirectConfig &lhs, const HttpRedirectConfig &rhs)
	{
		return !(lhs == rhs);
	}
};

}	 // namespace webserv

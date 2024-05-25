#pragma once

#include <string>
#include <types.hpp>

#include "../classDefUtils.hpp"

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
};

}	 // namespace webserv

#pragma once

#include <string>

#include "../classDefUtils.hpp"
#include "./IHttpRedirectConfig.hpp"

namespace webserv
{

class HttpRedirectConfig : public IHttpRedirectConfig
{
	DECL_VAR_REF_GETTER_SETTER(std::string, To)
	DECL_VAR_GETTER_SETTER(std::uint16_t, Code)

 public:
	HttpRedirectConfig();
	HttpRedirectConfig(
		const std::string& to,
		std::uint16_t code
	);
};

}	 // namespace webserv

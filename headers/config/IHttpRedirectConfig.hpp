#pragma once

#include <string>

#include "../classDefUtils.hpp"

namespace webserv
{
class IHttpRedirectConfig
{
 public:
	virtual ~IHttpRedirectConfig(){};

	DECL_PURE_VIRTUAL_REF_GETTER(std::string, To)
	DECL_PURE_VIRTUAL_GETTER(std::uint16_t, Code)

	virtual IHttpRedirectConfig *clone() const = 0;
};

}	 // namespace webserv

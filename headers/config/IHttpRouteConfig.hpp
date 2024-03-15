#pragma once

#include <string>

#include "../classDefUtils.hpp"
#include "./IHttpRedirectConfig.hpp"

namespace webserv
{

class IHttpRouteConfig
{
 public:
	virtual ~IHttpRouteConfig() {}

	DECL_PURE_VIRTUAL_REF_GETTER(std::vector<std::string>, Methods)
	DECL_PURE_VIRTUAL_GETTER(const IHttpRedirectConfig *, Redirect)

	virtual IHttpRouteConfig *clone() const = 0;
};

}	 // namespace webserv

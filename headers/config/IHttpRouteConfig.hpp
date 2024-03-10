#pragma once

#include <string>

#include "../classDefUtils.hpp"
#include "./HttpRedirectConfig.hpp"

namespace webserv
{

class IHttpRouteConfig
{
 public:
	virtual ~IHttpRouteConfig() {}

	DECL_PURE_VIRTUAL_REF_GETTER(std::vector<std::string>, Methods)
	DECL_PURE_VIRTUAL_REF_GETTER(HttpRedirectConfig, Redirect)
};

}	 // namespace webserv

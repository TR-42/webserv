#pragma once

#include <map>
#include <string>
#include <vector>

#include "../classDefUtils.hpp"
#include "./HttpRedirectConfig.hpp"
#include "./IHttpRouteConfig.hpp"

namespace webserv
{

class HttpRouteConfig : public IHttpRouteConfig
{
	DECL_VAR_REF_GETTER_SETTER(std::vector<std::string>, Methods)
	DECL_VAR_REF_GETTER_SETTER(HttpRedirectConfig, Redirect)

 public:
	HttpRouteConfig();
};

}	 // namespace webserv

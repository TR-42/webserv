#pragma once

#include <map>
#include <string>
#include <vector>

#include "../classDefUtils.hpp"
#include "./HttpRedirectConfig.hpp"

namespace webserv
{

class HttpRouteConfig
{
	DECL_VAR_REF_GETTER_SETTER(std::vector<std::string>, Methods)
	DECL_VAR_REF_GETTER_SETTER(HttpRedirectConfig, Redirect)

 private:
	void setProps(
		const std::vector<std::string> &methods,
		const HttpRedirectConfig &redirect
	);

 public:
	HttpRouteConfig();
	HttpRouteConfig(const HttpRouteConfig &from);
	virtual ~HttpRouteConfig();
	HttpRouteConfig &operator=(const HttpRouteConfig &from);
};

}	 // namespace webserv

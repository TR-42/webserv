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
	DECL_PTR_GETTER_SETTER(IHttpRedirectConfig, Redirect)

 private:
	void setProps(
		const std::vector<std::string> &methods,
		const IHttpRedirectConfig &redirect
	);

 public:
	HttpRouteConfig();
	HttpRouteConfig(const HttpRouteConfig &from);
	virtual ~HttpRouteConfig();
	HttpRouteConfig &operator=(const HttpRouteConfig &from);

	virtual IHttpRouteConfig *clone() const;
};

}	 // namespace webserv

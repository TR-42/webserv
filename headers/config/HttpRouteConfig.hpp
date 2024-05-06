#pragma once

#include <map>
#include <string>
#include <vector>

#include "../classDefUtils.hpp"
#include "./CgiConfig.hpp"
#include "./HttpRedirectConfig.hpp"

namespace webserv
{

typedef std::vector<CgiConfig> CgiConfigListType;

class HttpRouteConfig
{
	DECL_VAR_REF_GETTER_SETTER(std::string, RequestPath)
	DECL_VAR_REF_GETTER_SETTER(std::vector<std::string>, Methods)
	DECL_VAR_REF_GETTER_SETTER(HttpRedirectConfig, Redirect)
	DECL_VAR_REF_GETTER_SETTER(std::string, DocumentRoot)
	DECL_VAR_GETTER_SETTER(bool, IsDocumentListingEnabled)
	DECL_VAR_REF_GETTER_SETTER(std::vector<std::string>, IndexFileList)
	DECL_VAR_REF_GETTER_SETTER(CgiConfigListType, CgiConfigList)

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

#pragma once

#include <map>
#include <string>
#include <vector>

#include "../classDefUtils.hpp"
#include "./CgiConfig.hpp"
#include "./HttpRedirectConfig.hpp"

namespace webserv
{

typedef std::map<uint16_t, CgiConfig> CgiConfigMapType;

class HttpRouteConfig
{
	// TODO: リクエストパスの設定を追加する
	DECL_VAR_REF_GETTER_SETTER(std::vector<std::string>, Methods)
	DECL_VAR_REF_GETTER_SETTER(HttpRedirectConfig, Redirect)
	DECL_VAR_REF_GETTER_SETTER(std::string, DocumentRoot)
	DECL_VAR_GETTER_SETTER(bool, IsDocumentListingEnabled)
	DECL_VAR_REF_GETTER_SETTER(std::string, IndexFile)
	DECL_VAR_REF_GETTER_SETTER(CgiConfigMapType, CgiConfigMap)

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

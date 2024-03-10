#pragma once

#include <map>
#include <string>
#include <vector>

#include "../classDefUtils.hpp"
#include "./IHttpRouteConfig.hpp"
#include "./IServerConfig.hpp"

namespace webserv
{

class ServerConfig : public IServerConfig
{
 private:
	DECL_VAR_REF_GETTER_SETTER(std::vector<std::string>, ServerNameList)
	DECL_VAR_REF_GETTER_SETTER(std::string, Host)
	DECL_VAR_GETTER_SETTER(std::uint16_t, Port)
	DECL_VAR_REF_GETTER_SETTER(std::size_t, RequestBodyLimit)
	DECL_VAR_REF_GETTER_SETTER(ErrorPageMapType, ErrorPageMap)
	DECL_VAR_REF_GETTER_SETTER(std::vector<HttpRouteConfig>, RouteList)

 public:
	ServerConfig();
	ServerConfig(
		const std::vector<std::string>& serverNameList,
		const std::string& host,
		std::uint16_t port,
		std::size_t requestBodyLimit,
		const ErrorPageMapType& errorPages,
		const std::vector<HttpRouteConfig>& routeList
	);
};

}	 // namespace webserv

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
	DECL_VAR_REF_GETTER_SETTER(std::vector<std::string>, ServerNameList)
	DECL_VAR_REF_GETTER_SETTER(std::string, Host)
	DECL_VAR_GETTER_SETTER(std::uint16_t, Port)
	DECL_VAR_REF_GETTER_SETTER(std::size_t, RequestBodyLimit)
	DECL_VAR_REF_GETTER_SETTER(ErrorPageMapType, ErrorPageMap)
	DECL_VAR_REF_GETTER_SETTER(RouteListType, RouteList)

 private:
	void setProps(
		const std::vector<std::string> &serverNameList,
		const std::string &host,
		std::uint16_t port,
		std::size_t requestBodyLimit,
		const ErrorPageMapType &errorPages,
		const RouteListType &routeList
	);

 public:
	ServerConfig();
	ServerConfig(const ServerConfig &from);
	virtual ~ServerConfig();
	ServerConfig &operator=(const ServerConfig &from);

	ServerConfig(
		const std::vector<std::string> &serverNameList,
		const std::string &host,
		std::uint16_t port,
		std::size_t requestBodyLimit,
		const ErrorPageMapType &errorPages,
		const RouteListType &routeList
	);

	virtual IServerConfig *clone() const;
};

}	 // namespace webserv

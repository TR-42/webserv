#pragma once

#include <map>
#include <string>
#include <vector>

#include "../classDefUtils.hpp"
#include "./HttpRouteConfig.hpp"

#define EQ(name) (lhs._##name == rhs._##name)

namespace webserv
{

typedef std::map<uint16_t, std::string> ErrorPageMapType;
typedef std::vector<HttpRouteConfig> RouteListType;

class ServerConfig
{
	DECL_VAR_REF_GETTER_SETTER(std::vector<std::string>, ServerNameList)
	DECL_VAR_GETTER_SETTER(uint16_t, Port)
	DECL_VAR_GETTER_SETTER(size_t, TimeoutMs)
	DECL_VAR_REF_GETTER_SETTER(std::size_t, RequestBodyLimit)
	DECL_VAR_REF_GETTER_SETTER(ErrorPageMapType, ErrorPageMap)
	DECL_VAR_REF_GETTER_SETTER(RouteListType, RouteList)

 public:
	ServerConfig();
	ServerConfig(const ServerConfig &from);
	virtual ~ServerConfig();
	ServerConfig &operator=(const ServerConfig &from);

	ServerConfig(
		const std::vector<std::string> &serverNameList,
		uint16_t port,
		size_t timeoutMs,
		std::size_t requestBodyLimit,
		const ErrorPageMapType &errorPages,
		const RouteListType &routeList
	);

	friend bool operator==(const ServerConfig &lhs, const ServerConfig &rhs)
	{
		return (EQ(ServerNameList) && EQ(Port) && EQ(RequestBodyLimit) && EQ(ErrorPageMap) && EQ(RouteList));
	}

	friend bool operator!=(const ServerConfig &lhs, const ServerConfig &rhs)
	{
		return !(lhs == rhs);
	}
};

}	 // namespace webserv

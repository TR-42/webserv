#pragma once

#include <map>
#include <string>

#include "../classDefUtils.hpp"
#include "./HttpRouteConfig.hpp"

namespace webserv
{

typedef std::map<std::uint16_t, std::string> ErrorPageMapType;

class IServerConfig
{
 public:
	virtual ~IServerConfig() {}

	DECL_PURE_VIRTUAL_REF_GETTER(std::vector<std::string>, ServerNameList)
	DECL_PURE_VIRTUAL_REF_GETTER(std::string, Host)
	DECL_PURE_VIRTUAL_GETTER(std::uint16_t, Port)
	DECL_PURE_VIRTUAL_REF_GETTER(std::size_t, RequestBodyLimit)
	DECL_PURE_VIRTUAL_REF_GETTER(ErrorPageMapType, ErrorPageMap)
	DECL_PURE_VIRTUAL_REF_GETTER(std::vector<HttpRouteConfig>, RouteList)
};

}	 // namespace webserv

#include <config/ServerConfig.hpp>
#include <map>
#include <string>

namespace webserv
{

ServerConfig::ServerConfig(
) : _ServerNameList(),
		_Host(""),
		_Port(0),
		_RequestBodyLimit(0),
		_ErrorPageMap(),
		_RouteList()
{
}

ServerConfig::ServerConfig(
	const std::vector<std::string>& serverNameList,
	const std::string& host,
	std::uint16_t port,
	std::size_t requestBodyLimit,
	const ErrorPageMapType& errorPageMap,
	const std::vector<HttpRouteConfig>& routeList
) : _ServerNameList(serverNameList),
		_Host(host),
		_Port(port),
		_RequestBodyLimit(requestBodyLimit),
		_ErrorPageMap(errorPageMap),
		_RouteList(routeList)
{
}

IMPL_REF_GETTER_SETTER_NS(std::vector<std::string>, ServerNameList, ServerConfig::)
IMPL_REF_GETTER_SETTER_NS(std::string, Host, ServerConfig::)
IMPL_GETTER_SETTER_NS(std::uint16_t, Port, ServerConfig::)
IMPL_REF_GETTER_SETTER_NS(std::size_t, RequestBodyLimit, ServerConfig::)
IMPL_REF_GETTER_SETTER_NS(ErrorPageMapType, ErrorPageMap, ServerConfig::)
IMPL_REF_GETTER_SETTER_NS(std::vector<HttpRouteConfig>, RouteList, ServerConfig::)

}	 // namespace webserv

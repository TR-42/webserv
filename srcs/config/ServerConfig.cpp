#include <config/ServerConfig.hpp>
#include <map>
#include <string>

namespace webserv
{

ServerConfig::ServerConfig(
) : _ServerNameList(),
		_Port(0),
		_RequestBodyLimit(0),
		_ErrorPageMap(),
		_RouteList()
{
}

webserv::ServerConfig::ServerConfig(
	const ServerConfig &from
) : _ServerNameList(from._ServerNameList),
		_Port(from._Port),
		_RequestBodyLimit(from._RequestBodyLimit),
		_ErrorPageMap(from._ErrorPageMap),
		_RouteList(from._RouteList)
{
}

ServerConfig::~ServerConfig()
{
}

ServerConfig &webserv::ServerConfig::operator=(
	const ServerConfig &from
)
{
	if (this == &from)
		return *this;

	this->_ServerNameList = from._ServerNameList;
	this->_Port = from._Port;
	this->_RequestBodyLimit = from._RequestBodyLimit;
	this->_ErrorPageMap = from._ErrorPageMap;
	this->_RouteList = from._RouteList;

	return *this;
}

ServerConfig::ServerConfig(
	const std::vector<std::string> &serverNameList,
	uint16_t port,
	std::size_t requestBodyLimit,
	const ErrorPageMapType &errorPageMap,
	const RouteListType &routeList
) : _ServerNameList(serverNameList),
		_Port(port),
		_RequestBodyLimit(requestBodyLimit),
		_ErrorPageMap(errorPageMap),
		_RouteList(routeList)
{
}

IMPL_REF_GETTER_SETTER_NS(std::vector<std::string>, ServerNameList, ServerConfig::)
IMPL_GETTER_SETTER_NS(uint16_t, Port, ServerConfig::)
IMPL_REF_GETTER_SETTER_NS(std::size_t, RequestBodyLimit, ServerConfig::)
IMPL_REF_GETTER_SETTER_NS(ErrorPageMapType, ErrorPageMap, ServerConfig::)
IMPL_REF_GETTER_SETTER_NS(RouteListType, RouteList, ServerConfig::)

}	 // namespace webserv

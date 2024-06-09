#include <config/ServerConfig.hpp>
#include <map>
#include <string>

namespace webserv
{

ServerConfig::ServerConfig(
) : _Key(),
		_ServerNameList(),
		_Port(0),
		_TimeoutMs(100),
		_RequestBodyLimit(0),
		_ErrorPageMap(),
		_RouteList()
{
}

webserv::ServerConfig::ServerConfig(
	const ServerConfig &from
) : _Key(from._Key),
		_ServerNameList(from._ServerNameList),
		_Port(from._Port),
		_TimeoutMs(from._TimeoutMs),
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

	this->_Key = from._Key;
	this->_ServerNameList = from._ServerNameList;
	this->_Port = from._Port;
	this->_TimeoutMs = from._TimeoutMs;
	this->_RequestBodyLimit = from._RequestBodyLimit;
	this->_ErrorPageMap = from._ErrorPageMap;
	this->_RouteList = from._RouteList;

	return *this;
}

ServerConfig::ServerConfig(
	const std::string &key,
	const std::vector<std::string> &serverNameList,
	uint16_t port,
	size_t timeoutMs,
	std::size_t requestBodyLimit,
	const ErrorPageMapType &errorPageMap,
	const RouteListType &routeList
) : _Key(key),
		_ServerNameList(serverNameList),
		_Port(port),
		_TimeoutMs(timeoutMs),
		_RequestBodyLimit(requestBodyLimit),
		_ErrorPageMap(errorPageMap),
		_RouteList(routeList)
{
}

}	 // namespace webserv

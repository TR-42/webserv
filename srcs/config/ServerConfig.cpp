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

webserv::ServerConfig::ServerConfig(
	const ServerConfig &from
)
{
	*this = from;
}

ServerConfig::~ServerConfig()
{
	for (
		RouteListType::iterator itRouteList = this->_RouteList.begin();
		itRouteList != this->_RouteList.end();
		++itRouteList
	) {
		delete *itRouteList;
	}
	this->_RouteList.clear();
}

ServerConfig &webserv::ServerConfig::operator=(
	const ServerConfig &from
)
{
	if (this == &from)
		return *this;

	this->setProps(
		from._ServerNameList,
		from._Host,
		from._Port,
		from._RequestBodyLimit,
		from._ErrorPageMap,
		from._RouteList
	);

	return *this;
}

ServerConfig::ServerConfig(
	const std::vector<std::string> &serverNameList,
	const std::string &host,
	std::uint16_t port,
	std::size_t requestBodyLimit,
	const ErrorPageMapType &errorPageMap,
	const RouteListType &routeList
) : _ServerNameList(serverNameList),
		_Host(host),
		_Port(port),
		_RequestBodyLimit(requestBodyLimit),
		_ErrorPageMap(errorPageMap),
		_RouteList(routeList)
{
}

void ServerConfig::setProps(
	const std::vector<std::string> &serverNameList,
	const std::string &host,
	std::uint16_t port,
	std::size_t requestBodyLimit,
	const ErrorPageMapType &errorPages,
	const RouteListType &routeList
)
{
	this->_ServerNameList = serverNameList;
	this->_Host = host;
	this->_Port = port;
	this->_RequestBodyLimit = requestBodyLimit;
	this->_ErrorPageMap = errorPages;

	for (
		RouteListType::const_iterator itRouteList = routeList.begin();
		itRouteList != routeList.end();
		++itRouteList
	) {
		this->_RouteList.push_back((*itRouteList)->clone());
	}
}

IServerConfig *webserv::ServerConfig::clone() const
{
	return new ServerConfig(*this);
}

IMPL_REF_GETTER_SETTER_NS(std::vector<std::string>, ServerNameList, ServerConfig::)
IMPL_REF_GETTER_SETTER_NS(std::string, Host, ServerConfig::)
IMPL_GETTER_SETTER_NS(std::uint16_t, Port, ServerConfig::)
IMPL_REF_GETTER_SETTER_NS(std::size_t, RequestBodyLimit, ServerConfig::)
IMPL_REF_GETTER_SETTER_NS(ErrorPageMapType, ErrorPageMap, ServerConfig::)
IMPL_REF_GETTER_SETTER_NS(RouteListType, RouteList, ServerConfig::)

}	 // namespace webserv

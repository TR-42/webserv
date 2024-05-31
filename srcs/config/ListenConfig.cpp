#include <config/ListenConfig.hpp>

namespace webserv
{

ListenConfig::ListenConfig(
) : _ListenMap()
{
}

ListenConfig::ListenConfig(
	const ListenConfig &from
) : _ListenMap(from._ListenMap)
{
}

ListenConfig::ListenConfig(
	const ListenMapType &listenMap
) : _ListenMap(listenMap)
{
}

ListenConfig &ListenConfig::operator=(
	const ListenConfig &from
)
{
	if (this == &from)
		return *this;

	this->_ListenMap = from._ListenMap;

	return *this;
}

ListenConfig::~ListenConfig()
{
	this->_ListenMap.clear();
}

void ListenConfig::addServerConfig(
	const ServerConfig &serverConfig
)
{
	this->_ListenMap.at(serverConfig.getPort()).push_back(serverConfig);
}

}	 // namespace webserv

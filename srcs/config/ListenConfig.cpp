#include <config/ListenConfig.hpp>

namespace webserv
{

ListenConfig::ListenConfig(
) : _ListenMap()
{
}

ListenConfig::ListenConfig(
	const ListenMapType& listenMap
) : _ListenMap(listenMap)
{
}

void ListenConfig::addServerConfig(
	const ServerConfig& serverConfig
)
{
	this->_ListenMap.at(serverConfig.getPort()).push_back(serverConfig);
}

IMPL_REF_GETTER_SETTER_NS(ListenMapType, ListenMap, ListenConfig::)

}	 // namespace webserv

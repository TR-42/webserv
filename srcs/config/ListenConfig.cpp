#include <config/ListenConfig.hpp>

namespace webserv
{

ListenConfig::ListenConfig(
) : _ListenMap()
{
}

ListenConfig::ListenConfig(
	const ListenConfig &from
)
{
	*this = from;
}

ListenConfig::ListenConfig(
	const ListenMapType &listenMap
)
{
	this->setProps(
		listenMap
	);
}

ListenConfig &ListenConfig::operator=(
	const ListenConfig &from
)
{
	if (this == &from)
		return *this;

	this->setProps(
		from._ListenMap
	);

	return *this;
}

void webserv::ListenConfig::setProps(
	const ListenMapType &srcListenMap
)
{
	this->_ListenMap = srcListenMap;
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

IMPL_REF_GETTER_SETTER_NS(ListenMapType, ListenMap, ListenConfig::)

}	 // namespace webserv

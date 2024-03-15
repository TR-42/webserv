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
	for (
		ListenMapType::const_iterator itSrcListenMap = srcListenMap.begin();
		itSrcListenMap != srcListenMap.end();
		++itSrcListenMap
	) {
		const ServerConfigListType &srcServerConfigList = itSrcListenMap->second;
		ServerConfigListType dstServerConfigList;

		for (
			ServerConfigListType::const_iterator itSrcServerConfigList = srcServerConfigList.begin();
			itSrcServerConfigList != srcServerConfigList.end();
			++itSrcServerConfigList
		) {
			dstServerConfigList.push_back((*itSrcServerConfigList)->clone());
		}

		this->_ListenMap.insert(std::make_pair(itSrcListenMap->first, dstServerConfigList));
	}
}

ListenConfig::~ListenConfig()
{
	for (
		ListenMapType::iterator itListenMap = this->_ListenMap.begin();
		itListenMap != this->_ListenMap.end();
		++itListenMap
	) {
		for (
			ServerConfigListType::iterator itServerConfigList = itListenMap->second.begin();
			itServerConfigList != itListenMap->second.end();
			++itServerConfigList
		) {
			delete *itServerConfigList;
		}

		itListenMap->second.clear();
	}

	this->_ListenMap.clear();
}

void ListenConfig::addServerConfig(
	const IServerConfig &serverConfig
)
{
	this->_ListenMap.at(serverConfig.getPort()).push_back(serverConfig.clone());
}

IListenConfig *ListenConfig::clone() const
{
	return new ListenConfig(*this);
}

IMPL_REF_GETTER_SETTER_NS(ListenMapType, ListenMap, ListenConfig::)

}	 // namespace webserv

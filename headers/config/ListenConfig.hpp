#pragma once

#include <map>

#include "./ServerConfig.hpp"
#include "./classDefUtils.hpp"

namespace webserv
{

typedef std::vector<ServerConfig> ServerConfigListType;
typedef std::map<uint16_t, ServerConfigListType> ListenMapType;

class ListenConfig
{
 private:
	void setProps(
		const ListenMapType &srcListenMap
	);

 public:
	ListenConfig();
	ListenConfig(
		const ListenConfig &from
	);
	~ListenConfig();
	ListenConfig &operator=(
		const ListenConfig &from
	);

	ListenConfig(
		const ListenMapType &listenMap
	);

	DECL_VAR_REF_GETTER_SETTER(ListenMapType, ListenMap)

	void addServerConfig(
		const ServerConfig &serverConfig
	);
};

}	 // namespace webserv

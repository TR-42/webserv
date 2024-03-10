#pragma once

#include <map>

#include "./IListenConfig.hpp"
#include "./classDefUtils.hpp"

namespace webserv
{

class ListenConfig : public IListenConfig
{
 public:
	ListenConfig();
	ListenConfig(
		const ListenMapType& listenMap
	);

	DECL_VAR_REF_GETTER_SETTER(ListenMapType, ListenMap)

	void addServerConfig(
		const ServerConfig& serverConfig
	);
};

}	 // namespace webserv

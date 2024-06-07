#pragma once

#include <map>

#include "./ServerConfig.hpp"
#include "./classDefUtils.hpp"

#define EQ(name) (lhs._##name == rhs._##name)

namespace webserv
{

typedef std::vector<ServerConfig> ServerConfigListType;
typedef std::map<uint16_t, ServerConfigListType> ListenMapType;

class ListenConfig
{
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

	friend bool operator==(const ListenConfig &lhs, const ListenConfig &rhs)
	{
		return (EQ(ListenMap));
	}

	friend bool operator!=(const ListenConfig &lhs, const ListenConfig &rhs)
	{
		return !(lhs == rhs);
	}
};

}	 // namespace webserv

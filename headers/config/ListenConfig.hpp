#pragma once

#include <map>

#include "./IListenConfig.hpp"
#include "./classDefUtils.hpp"

namespace webserv
{

class ListenConfig : public IListenConfig
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
		const IServerConfig &serverConfig
	);
	IListenConfig *clone() const;
};

}	 // namespace webserv

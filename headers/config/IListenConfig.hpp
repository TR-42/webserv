#pragma once

#include <map>

#include "./IServerConfig.hpp"
#include "./classDefUtils.hpp"

namespace webserv
{

typedef std::vector<const IServerConfig *> ServerConfigListType;
typedef std::map<std::uint16_t, ServerConfigListType> ListenMapType;

class IListenConfig
{
 public:
	virtual ~IListenConfig() {}
	DECL_PURE_VIRTUAL_REF_GETTER(ListenMapType, ListenMap)

	virtual IListenConfig *clone() const = 0;
};

}	 // namespace webserv

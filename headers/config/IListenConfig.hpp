#pragma once

#include <map>

#include "./ServerConfig.hpp"
#include "./classDefUtils.hpp"

namespace webserv
{

typedef std::map<std::uint16_t, std::vector<ServerConfig> > ListenMapType;

class IListenConfig
{
 public:
	virtual ~IListenConfig() {}
	DECL_PURE_VIRTUAL_REF_GETTER(ListenMapType, ListenMap)
};

}	 // namespace webserv

#pragma once
#include <classDefUtils.hpp>
#include <map>
#include <string>
#include <vector>

namespace webserv
{

typedef std::map<std::string, std::vector<std::string> > ResponseHeaderMap;

class HttpResponse
{
	DECL_VAR_GETTER_SETTER(std::string, Version)
	DECL_VAR_GETTER_SETTER(std::string, StatusCode)
	DECL_VAR_GETTER_SETTER(std::string, ReasonPhrase)
	DECL_VAR_GETTER_SETTER(ResponseHeaderMap, Headers)
	DECL_VAR_GETTER_SETTER(std::vector<uint8_t>, Body)

 public:
	std::vector<u_int8_t> generateResponsePacket() const;
};

}	 // namespace webserv

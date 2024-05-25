#pragma once

#include <classDefUtils.hpp>
#include <string>
#include <types.hpp>
#include <vector>

#include "http/HttpFieldMap.hpp"

namespace webserv
{

class HttpResponse
{
	DECL_VAR_GETTER_SETTER(std::string, Version)
	DECL_VAR_GETTER_SETTER(std::string, StatusCode)
	DECL_VAR_GETTER_SETTER(std::string, ReasonPhrase)
	DECL_VAR_REF_NO_CONST_GETTER_SETTER(HttpFieldMap, Headers)
	DECL_VAR_REF_NO_CONST_GETTER_SETTER(std::vector<uint8_t>, Body)

 public:
	std::vector<uint8_t> generateResponsePacket(bool withBody) const;
	void setBody(const std::string &body);
	HttpResponse();
	HttpResponse(const HttpResponse &src);
	HttpResponse &operator=(const HttpResponse &src);
	virtual ~HttpResponse();
};

}	 // namespace webserv

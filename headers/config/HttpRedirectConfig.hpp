#pragma once

#include <string>

#include "../classDefUtils.hpp"
#include "./IHttpRedirectConfig.hpp"

namespace webserv
{

class HttpRedirectConfig : public IHttpRedirectConfig
{
	DECL_VAR_REF_GETTER_SETTER(std::string, To)
	DECL_VAR_GETTER_SETTER(std::uint16_t, Code)

 private:
	void setProps(
		const std::string &to,
		std::uint16_t code
	);

 public:
	HttpRedirectConfig();
	HttpRedirectConfig(
		const HttpRedirectConfig &from
	);
	virtual ~HttpRedirectConfig();
	HttpRedirectConfig &operator=(
		const HttpRedirectConfig &from
	);

	HttpRedirectConfig(
		const std::string &to,
		std::uint16_t code
	);

	virtual IHttpRedirectConfig *clone() const;
};

}	 // namespace webserv

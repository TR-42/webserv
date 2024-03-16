#pragma once

#include <string>

#include "../classDefUtils.hpp"

namespace webserv
{

class CgiConfig
{
	DECL_VAR_REF_GETTER_SETTER(std::string, ExtensionWithoutDot)
	DECL_VAR_REF_GETTER_SETTER(std::string, CgiExecutableFullPath)

 private:
	void setProps(
		const std::string &extensionWithoutDot,
		const std::string &cgiExecutableFullPath
	);

 public:
	CgiConfig();
	CgiConfig(
		const CgiConfig &from
	);
	virtual ~CgiConfig();
	CgiConfig &operator=(
		const CgiConfig &from
	);

	CgiConfig(
		const std::string &extensionWithoutDot,
		const std::string &cgiExecutableFullPath
	);
};

}	 // namespace webserv

#pragma once

#include <EnvManager.hpp>
#include <string>

#include "../classDefUtils.hpp"

namespace webserv
{

class CgiConfig
{
	DECL_VAR_REF_GETTER_SETTER(std::string, ExtensionWithoutDot)
	DECL_VAR_REF_GETTER_SETTER(std::string, CgiExecutableFullPath)
	DECL_VAR_REF_GETTER_SETTER(env::EnvManager, EnvPreset)

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
		const std::string &cgiExecutableFullPath,
		const env::EnvManager &envPreset
	);
};

}	 // namespace webserv

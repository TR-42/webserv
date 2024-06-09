#pragma once

#include <EnvManager.hpp>
#include <string>

#include "../classDefUtils.hpp"

#define EQ(name) (lhs._##name == rhs._##name)

namespace webserv
{

class CgiConfig
{
	DECL_VAR_REF_GETTER_SETTER(std::string, Key)
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
		const std::string &key,
		const std::string &extensionWithoutDot,
		const std::string &cgiExecutableFullPath,
		const env::EnvManager &envPreset
	);

	friend bool operator==(const CgiConfig &lhs, const CgiConfig &rhs)
	{
		return (EQ(Key) && EQ(ExtensionWithoutDot) && EQ(CgiExecutableFullPath) && EQ(EnvPreset));
	}

	friend bool operator!=(const CgiConfig &lhs, const CgiConfig &rhs)
	{
		return !(lhs == rhs);
	}
};

}	 // namespace webserv

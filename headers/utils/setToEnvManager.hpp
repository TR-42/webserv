#pragma once

#include <EnvManager.hpp>
#include <http/HttpFieldMap.hpp>

namespace webserv
{

namespace utils
{

void setToEnvManager(
	env::EnvManager &envManager,
	const HttpFieldMap &fieldMap
);

}	 // namespace utils

}	 // namespace webserv

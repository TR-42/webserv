#pragma once

#include "./ListenConfig.hpp"

namespace webserv
{

std::vector<std::string> validateConfig(
	const ListenConfig &listenConfigs
);

}	 // namespace webserv

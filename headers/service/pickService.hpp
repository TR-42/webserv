#pragma once

#include <config/ListenConfig.hpp>
#include <config/ServerRunningConfig.hpp>
#include <poll/Pollable.hpp>
#include <vector>

#include "./ServiceBase.hpp"

namespace webserv
{

ServiceBase *pickService(
	const ServerRunningConfigListType &listenConfigList,
	const HttpRequest &request,
	std::vector<Pollable *> &pollableList,
	const Logger &logger
);

}	 // namespace webserv

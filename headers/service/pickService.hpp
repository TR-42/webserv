#pragma once

#include <arpa/inet.h>

#include <config/ListenConfig.hpp>
#include <config/ServerRunningConfig.hpp>
#include <poll/Pollable.hpp>
#include <vector>

#include "./ServiceBase.hpp"

namespace webserv
{

ServiceBase *pickService(
	const struct sockaddr &clientAddr,
	const HttpRequest &request,
	std::vector<Pollable *> &pollableList,
	const Logger &logger
);

}	 // namespace webserv

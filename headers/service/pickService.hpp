#pragma once

#include <http/HttpRequest.hpp>
#include <poll/Pollable.hpp>
#include <vector>

#include "./ServiceBase.hpp"

namespace webserv
{

ServiceBase *pickService(
	const HttpRequest &request,
	std::vector<Pollable *> &pollableList,
	const Logger &logger
);

}	 // namespace webserv

#pragma once

#include <config/HttpRouteConfig.hpp>

namespace webserv
{

std::string getRequestedFilePath(
	const HttpRouteConfig &routeConfig,
	const std::string &requestPath
);

}	 // namespace webserv

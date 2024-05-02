#pragma once

#include <config/ListenConfig.hpp>

#include "./ServiceBase.hpp"

namespace webserv
{

ServiceBase *pickService(
	const ServerConfigListType &listenConfigList,
	const HttpRequest &request,
	const utils::ErrorPageProvider &errorPageProvider,
	const Logger &logger
);

}	 // namespace webserv

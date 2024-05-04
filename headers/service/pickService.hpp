#pragma once

#include <config/ListenConfig.hpp>
#include <config/ServerRunningConfig.hpp>

#include "./ServiceBase.hpp"

namespace webserv
{

ServiceBase *pickService(
	const ServerRunningConfigListType &listenConfigList,
	const HttpRequest &request,
	const utils::ErrorPageProvider &errorPageProvider,
	const Logger &logger
);

}	 // namespace webserv

#include <config/ListenConfig.hpp>
#include <config/ServerRunningConfig.hpp>
#include <service/DeleteFileService.hpp>
#include <service/GetFileService.hpp>
#include <service/ServiceBase.hpp>
#include <service/SimpleService.hpp>
#include <service/pickService.hpp>
#include <stdexcept>

namespace webserv
{

static const ServerRunningConfig pickServerConfig(
	const ServerRunningConfigListType &listenConfigList,
	const HttpRequest &request,
	const Logger &logger
)
{
	if (listenConfigList.empty()) {
		L_FATAL("No ServerConfig found");
		throw std::runtime_error("No ServerConfig found");
	}

	if (request.getHost().empty()) {
		return listenConfigList[0];
	}

	for (
		ServerRunningConfigListType::const_iterator itConfig = listenConfigList.begin();
		itConfig != listenConfigList.end();
		++itConfig
	) {
		if (itConfig->isServerNameMatch(request)) {
			return *itConfig;
		}
	}

	// Hostが一致するServerConfigが見つからなかった場合、一番最初に記述されていた設定に従う
	return listenConfigList[0];
};

static ServiceBase *pickService(
	const HttpRouteConfig &routeConfig,
	const HttpRequest &request,
	const utils::ErrorPageProvider &errorPageProvider,
	const Logger &logger
)
{
	LS_DEBUG()
		<< "Picking service for route: " << routeConfig.getRequestPath()
		<< std::endl;

	// TODO: RouteによるServiceの選択
	if (request.getMethod() == "GET") {
		return new GetFileService(
			request,
			errorPageProvider,
			logger
		);
	} else if (request.getMethod() == "DELETE") {
		return new DeleteFileService(
			request,
			errorPageProvider,
			logger
		);
	} else {
		return NULL;
	}
}

ServiceBase *pickService(
	const ServerRunningConfigListType &listenConfigList,
	const HttpRequest &request,
	const Logger &logger
)
{
	ServerRunningConfig serverConfig = pickServerConfig(
		listenConfigList,
		request,
		logger
	);

	HttpRouteConfig routeConfig = serverConfig.pickRouteConfig(request);
	return pickService(
		routeConfig,
		request,
		serverConfig.getErrorPageProvider(),
		logger
	);
}

}	 // namespace webserv

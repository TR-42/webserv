#include <config/ListenConfig.hpp>
#include <service/ServiceBase.hpp>
#include <service/SimpleService.hpp>
#include <service/pickService.hpp>

namespace webserv
{

const ServerConfig pickServerConfig(
	const ServerConfigListType &listenConfigList,
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
		ServerConfigListType::const_iterator it = listenConfigList.begin();
		it != listenConfigList.end();
		++it
	) {
		ServerConfig serverConfig = *it;
		if (serverConfig.getHost() == request.getHost()) {
			return serverConfig;
		}
	}

	// Hostが一致するServerConfigが見つからなかった場合、一番最初に記述されていた設定に従う
	return listenConfigList[0];
};

static bool isMethodFound(
	const std::vector<std::string> &methodList,
	const std::string &method
)
{
	if (methodList.empty()) {
		return false;
	}

	for (
		std::vector<std::string>::const_iterator it = methodList.begin();
		it != methodList.end();
		++it
	) {
		if (method == *it) {
			return true;
		}
	}
	return false;
}

static bool pickRouteConfig(
	const RouteListType &routeList,
	const HttpRequest &request,
	HttpRouteConfig &dstRouteConfig,
	const Logger &logger
)
{
	(void)logger;

	for (
		RouteListType::const_iterator routeIter = routeList.begin();
		routeIter != routeList.end();
		++routeIter
	) {
		if (!isMethodFound(routeIter->getMethods(), request.getMethod())) {
			continue;
		}

		// TODO: パスに応じたRouteConfigの選択
		dstRouteConfig = *routeIter;
		return true;
	}

	return false;
}

static ServiceBase *pickService(
	const HttpRouteConfig &routeConfig,
	const HttpRequest &request,
	const utils::ErrorPageProvider &errorPageProvider,
	const Logger &logger
)
{
	(void)routeConfig;
	// TODO: RouteによるServiceの選択
	return new SimpleService(
		request,
		errorPageProvider,
		logger
	);
}

ServiceBase *pickService(
	const ServerConfigListType &listenConfigList,
	const HttpRequest &request,
	const utils::ErrorPageProvider &errorPageProvider,
	const Logger &logger
)
{
	ServerConfig serverConfig = pickServerConfig(
		listenConfigList,
		request,
		logger
	);

	HttpRouteConfig routeConfig;
	bool isRouteConfigFound = pickRouteConfig(
		serverConfig.getRouteList(),
		request,
		routeConfig,
		logger
	);
	if (!isRouteConfigFound) {
		L_ERROR("No RouteConfig found");
		return NULL;
	}

	return pickService(
		routeConfig,
		request,
		errorPageProvider,
		logger
	);
}

}	 // namespace webserv

#include <config/ListenConfig.hpp>
#include <service/DeleteFileService.hpp>
#include <service/GetFileService.hpp>
#include <service/ServiceBase.hpp>
#include <service/SimpleService.hpp>
#include <service/pickService.hpp>
#include <stdexcept>

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
	(void)errorPageProvider;
	// TODO: RouteによるServiceの選択
	if (request.getMethod() == "GET") {
		return new GetFileService(
			request,
			utils::ErrorPageProvider(),
			logger
		);
	} else if (request.getMethod() == "DELETE") {
		return new DeleteFileService(
			request,
			utils::ErrorPageProvider(),
			logger
		);
	} else {
		return NULL;
	}
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
		// routeConfigが未実装のため、一旦コメントアウト
		// return NULL;
	}

	return pickService(
		routeConfig,
		request,
		errorPageProvider,
		logger
	);
}

}	 // namespace webserv

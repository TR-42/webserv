#include <config/ListenConfig.hpp>
#include <config/ServerRunningConfig.hpp>
#include <service/CgiService.hpp>
#include <service/DeleteFileService.hpp>
#include <service/GetFileService.hpp>
#include <service/PostFileService.hpp>
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
	std::vector<Pollable *> &pollableList,
	const Logger &logger
)
{
	LS_DEBUG()
		<< "Picking service for route: " << routeConfig.getRequestPath()
		<< std::endl;

	// TODO: お遊び設定を消す
	if (routeConfig.getRequestPath() == "/simple") {
		L_INFO("SimpleService selected");
		return new SimpleService(
			request,
			routeConfig,
			errorPageProvider,
			logger
		);
	} else if (routeConfig.getRequestPath() == "/resources/php-cgi") {
		L_INFO("CgiService selected");
		// TODO: CGI設定も適切に選択する
		const CgiConfig &cgiConfig = routeConfig.getCgiConfigList()[0];
		return new CgiService(
			request,
			cgiConfig.getCgiExecutableFullPath(),
			errorPageProvider,
			cgiConfig.getEnvPreset(),
			logger,
			pollableList
		);
	}

	// TODO: RouteによるServiceの選択 (特にCGI対応)
	if (request.getMethod() == "GET") {
		L_INFO("GetFileService selected");
		return new GetFileService(
			request,
			routeConfig,
			errorPageProvider,
			logger
		);
	} else if (request.getMethod() == "DELETE") {
		L_INFO("DeleteFileService selected");
		return new DeleteFileService(
			request,
			routeConfig,
			errorPageProvider,
			logger
		);
	} else if (request.getMethod() == "POST") {
		return new PostFileService(
			request,
			utils::ErrorPageProvider(),
			logger
		);
	} else {
		return NULL;
	}
}

ServiceBase *pickService(
	const ServerRunningConfigListType &listenConfigList,
	const HttpRequest &request,
	std::vector<Pollable *> &pollableList,
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
		pollableList,
		logger
	);
}

}	 // namespace webserv

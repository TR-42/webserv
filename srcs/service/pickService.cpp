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

static ServiceBase *pickService(
	const HttpRouteConfig &routeConfig,
	const HttpRequest &request,
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
			request.getServerRunningConfig().getErrorPageProvider(),
			logger
		);
	} else if (routeConfig.getRequestPath() == "/resources/php-cgi") {
		L_INFO("PhpCgiService selected");
		// TODO: CGI設定も適切に選択する
		const CgiConfig &cgiConfig = routeConfig.getCgiConfigList()[0];
		return new CgiService(
			request,
			cgiConfig.getCgiExecutableFullPath(),
			request.getServerRunningConfig().getErrorPageProvider(),
			cgiConfig.getEnvPreset(),
			logger,
			pollableList
		);
	} else if (routeConfig.getRequestPath() == "/resources/sh-cgi") {
		L_INFO("ShCgiService selected");
		// TODO: CGI設定も適切に選択する
		const CgiConfig &cgiConfig = routeConfig.getCgiConfigList()[0];
		return new CgiService(
			request,
			cgiConfig.getCgiExecutableFullPath(),
			request.getServerRunningConfig().getErrorPageProvider(),
			cgiConfig.getEnvPreset(),
			logger,
			pollableList
		);
	}

	// TODO: RouteによるServiceの選択 (特にCGI対応)
	if (request.getMethod() == "GET" || request.getMethod() == "HEAD") {
		L_INFO("GetFileService selected");
		return new GetFileService(
			request,
			routeConfig,
			request.getServerRunningConfig().getErrorPageProvider(),
			logger
		);
	} else if (request.getMethod() == "DELETE") {
		L_INFO("DeleteFileService selected");
		return new DeleteFileService(
			request,
			routeConfig,
			request.getServerRunningConfig().getErrorPageProvider(),
			logger
		);
	} else if (request.getMethod() == "POST") {
		return new PostFileService(
			request,
			request.getServerRunningConfig().getErrorPageProvider(),
			logger
		);
	} else {
		return NULL;
	}
}

ServiceBase *pickService(
	const HttpRequest &request,
	std::vector<Pollable *> &pollableList,
	const Logger &logger
)
{
	HttpRouteConfig routeConfig = request.getServerRunningConfig().pickRouteConfig(
		request.getPath()
	);
	return pickService(
		routeConfig,
		request,
		pollableList,
		logger
	);
}

}	 // namespace webserv

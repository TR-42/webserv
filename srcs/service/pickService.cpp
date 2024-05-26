#include <config/ListenConfig.hpp>
#include <config/ServerRunningConfig.hpp>
#include <service/CgiService.hpp>
#include <service/DeleteFileService.hpp>
#include <service/GetFileService.hpp>
#include <service/PostFileService.hpp>
#include <service/RequestedFileInfo.hpp>
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

	RequestedFileInfo requestedFileInfo(
		request.getPathSegmentList(),
		request.getPath().back() == '/',
		routeConfig,
		logger
	);

	if (requestedFileInfo.getIsNotFound()) {
		L_INFO("NotFound -> SimpleService selected");
		return new SimpleService(
			request,
			errorPageProvider.notFound(),
			errorPageProvider,
			logger
		);
	}

	if (requestedFileInfo.getIsCgi()) {
		L_INFO("CgiService selected");
		const CgiConfig &cgiConfig = requestedFileInfo.getCgiConfig();
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
	if (request.getMethod() == "GET" || request.getMethod() == "HEAD") {
		L_INFO("GetFileService selected");
		return new GetFileService(
			request,
			routeConfig,
			errorPageProvider,
			logger
		);
	}

	// ディレクトリ宛のリクエストはGET(HEAD)のみ対応
	if (requestedFileInfo.getIsDirectory()) {
		L_INFO("Directory but not GET/HEAD -> SimpleService selected");
		return new SimpleService(
			request,
			errorPageProvider.methodNotAllowed(),
			errorPageProvider,
			logger
		);
	}

	if (request.getMethod() == "DELETE") {
		L_INFO("DeleteFileService selected");
		return new DeleteFileService(
			request,
			routeConfig,
			errorPageProvider,
			logger
		);
	} else if (request.getMethod() == "POST") {
		L_INFO("PostFileService selected");
		return new PostFileService(
			request,
			utils::ErrorPageProvider(),
			logger
		);
	} else {
		L_INFO("Method not implemented -> NULL selected");
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

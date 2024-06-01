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

static ServiceBase *pickService(
	uint16_t serverPort,
	const HttpRouteConfig &routeConfig,
	const struct sockaddr &clientAddr,
	const HttpRequest &request,
	std::vector<Pollable *> &pollableList,
	const Logger &logger
)
{
	LS_DEBUG()
		<< "Picking service for route: " << routeConfig.getRequestPath()
		<< std::endl;

	RequestedFileInfo requestedFileInfo(
		request.getPathSegmentList(),
		request.getPath()[request.getPath().length() - 1] == '/',
		routeConfig,
		logger
	);

	if (requestedFileInfo.getIsNotFound()) {
		if (request.getMethod() == "POST") {
			L_INFO("NotFound && POST -> PostFileService selected");
			return new PostFileService(
				request,
				requestedFileInfo,
				request.getServerRunningConfig().getErrorPageProvider(),
				logger
			);
		} else {
			L_INFO("NotFound -> SimpleService selected");
			return new SimpleService(
				request,
				request.getServerRunningConfig().getErrorPageProvider().notFound(),
				request.getServerRunningConfig().getErrorPageProvider(),
				logger
			);
		}
	}

	if (requestedFileInfo.getIsCgi()) {
		L_INFO("CgiService selected");
		return new CgiService(
			request,
			requestedFileInfo,
			serverPort,
			clientAddr,
			request.getServerRunningConfig().getErrorPageProvider(),
			logger,
			pollableList
		);
	}

	if (request.getMethod() == "GET" || request.getMethod() == "HEAD") {
		L_INFO("GetFileService selected");
		return new GetFileService(
			request,
			requestedFileInfo,
			request.getServerRunningConfig().getErrorPageProvider(),
			logger
		);
	}

	// ディレクトリ宛のリクエストはGET(HEAD)のみ対応
	if (requestedFileInfo.getIsDirectory()) {
		L_INFO("Directory but not GET/HEAD -> SimpleService selected");
		return new SimpleService(
			request,
			request.getServerRunningConfig().getErrorPageProvider().methodNotAllowed(),
			request.getServerRunningConfig().getErrorPageProvider(),
			logger
		);
	}

	if (request.getMethod() == "DELETE") {
		L_INFO("DeleteFileService selected");
		return new DeleteFileService(
			request,
			requestedFileInfo,
			request.getServerRunningConfig().getErrorPageProvider(),
			logger
		);
	} else if (request.getMethod() == "POST") {
		L_INFO("PostFileService selected");
		return new PostFileService(
			request,
			requestedFileInfo,
			request.getServerRunningConfig().getErrorPageProvider(),
			logger
		);
	} else {
		L_INFO("Method not implemented -> NULL selected");
		return NULL;
	}
}

ServiceBase *pickService(
	const struct sockaddr &clientAddr,
	const HttpRequest &request,
	std::vector<Pollable *> &pollableList,
	const Logger &logger
)
{
	HttpRouteConfig routeConfig = request.getServerRunningConfig().pickRouteConfig(
		request.getPathSegmentList()
	);
	return pickService(
		request.getServerRunningConfig().getPort(),
		routeConfig,
		clientAddr,
		request,
		pollableList,
		logger
	);
}

}	 // namespace webserv

#include <sys/stat.h>

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

ServiceBase *pickService(
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

	const RequestedFileInfo &requestedFileInfo = request.getRequestedFileInfo();

	if (requestedFileInfo.getIsCgi()) {
		L_INFO("CgiService selected");
		return new CgiService(
			request,
			serverPort,
			clientAddr,
			logger,
			pollableList
		);
	}

	if (requestedFileInfo.getIsNotFound()) {
		if (request.getMethod() == "POST") {
			L_INFO("NotFound && POST -> PostFileService selected");
			return new PostFileService(
				request,
				logger
			);
		} else {
			L_INFO("NotFound -> SimpleService selected");
			return new SimpleService(
				request,
				request.getServerRunningConfig().getErrorPageProvider().notFound(),
				logger
			);
		}
	}

	if (request.getMethod() == "GET" || request.getMethod() == "HEAD") {
		L_INFO("GetFileService selected");
		if (requestedFileInfo.getStatBuf().st_mode & S_IRUSR) {
			return new GetFileService(
				request,
				logger
			);
		} else {
			L_INFO("Permission denied");
			return new SimpleService(
				request,
				request.getServerRunningConfig().getErrorPageProvider().permissionDenied(),
				logger
			);
		}
	}

	// ディレクトリ宛のリクエストはGET(HEAD)のみ対応
	if (requestedFileInfo.getIsDirectory()) {
		L_INFO("Directory but not GET/HEAD -> SimpleService selected");
		return new SimpleService(
			request,
			request.getServerRunningConfig().getErrorPageProvider().methodNotAllowed(),
			logger
		);
	}

	if (request.getMethod() == "DELETE") {
		L_INFO("DeleteFileService selected");
		// 本当は書き込み権限が無くても削除できるが、仕様として「書き込み権限がある場合のみ削除可能」とする
		if (requestedFileInfo.getStatBuf().st_mode & S_IWUSR) {
			return new DeleteFileService(
				request,
				logger
			);
		} else {
			L_INFO("Permission denied");
			return new SimpleService(
				request,
				request.getServerRunningConfig().getErrorPageProvider().permissionDenied(),
				logger
			);
		}
	} else if (request.getMethod() == "POST" || request.getMethod() == "PUT") {
		L_INFO("PostFileService selected");
		if (requestedFileInfo.getStatBuf().st_mode & S_IWUSR) {
			return new PostFileService(
				request,
				logger
			);
		} else {
			L_INFO("Permission denied");
			return new SimpleService(
				request,
				request.getServerRunningConfig().getErrorPageProvider().permissionDenied(),
				logger
			);
		}
	} else {
		L_INFO("Method not implemented -> NULL selected");
		return NULL;
	}
}

}	 // namespace webserv

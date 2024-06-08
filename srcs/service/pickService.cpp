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
		bool isExecutable = (requestedFileInfo.getStatBuf().st_mode & (S_IRUSR | S_IXUSR)) == (S_IRUSR | S_IXUSR);
		if (isExecutable) {
			return new CgiService(
				request,
				requestedFileInfo,
				serverPort,
				clientAddr,
				request.getServerRunningConfig().getErrorPageProvider(),
				logger,
				pollableList
			);
		} else {
			L_INFO("Permission denied");
			return new SimpleService(
				request,
				request.getServerRunningConfig().getErrorPageProvider().permissionDenied(),
				request.getServerRunningConfig().getErrorPageProvider(),
				logger
			);
		}
	}

	if (request.getMethod() == "GET" || request.getMethod() == "HEAD") {
		L_INFO("GetFileService selected");
		if (requestedFileInfo.getStatBuf().st_mode & S_IRUSR) {
			return new GetFileService(
				request,
				requestedFileInfo,
				request.getServerRunningConfig().getErrorPageProvider(),
				logger
			);
		} else {
			L_INFO("Permission denied");
			return new SimpleService(
				request,
				request.getServerRunningConfig().getErrorPageProvider().permissionDenied(),
				request.getServerRunningConfig().getErrorPageProvider(),
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
			request.getServerRunningConfig().getErrorPageProvider(),
			logger
		);
	}

	if (request.getMethod() == "DELETE") {
		L_INFO("DeleteFileService selected");
		// 本当は書き込み権限が無くても削除できるが、仕様として「書き込み権限がある場合のみ削除可能」とする
		if (requestedFileInfo.getStatBuf().st_mode & S_IWUSR) {
			return new DeleteFileService(
				request,
				requestedFileInfo,
				request.getServerRunningConfig().getErrorPageProvider(),
				logger
			);
		} else {
			L_INFO("Permission denied");
			return new SimpleService(
				request,
				request.getServerRunningConfig().getErrorPageProvider().permissionDenied(),
				request.getServerRunningConfig().getErrorPageProvider(),
				logger
			);
		}
	} else if (request.getMethod() == "POST") {
		L_INFO("PostFileService selected");
		if (requestedFileInfo.getStatBuf().st_mode & S_IWUSR) {
			return new PostFileService(
				request,
				requestedFileInfo,
				request.getServerRunningConfig().getErrorPageProvider(),
				logger
			);
		} else {
			L_INFO("Permission denied");
			return new SimpleService(
				request,
				request.getServerRunningConfig().getErrorPageProvider().permissionDenied(),
				request.getServerRunningConfig().getErrorPageProvider(),
				logger
			);
		}
	} else {
		L_INFO("Method not implemented -> NULL selected");
		return NULL;
	}
}

}	 // namespace webserv

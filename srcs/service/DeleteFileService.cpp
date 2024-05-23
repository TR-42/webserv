#include <errno.h>
#include <sys/stat.h>

#include <Logger.hpp>
#include <cstring>
#include <iostream>
#include <service/DeleteFileService.hpp>
#include <service/getRequestedFilePath.hpp>
#include <utils/getTimeStr.hpp>
#include <utils/modeToString.hpp>

namespace webserv
{

DeleteFileService::DeleteFileService(
	const HttpRequest &request,
	const HttpRouteConfig &routeConfig,
	const webserv::utils::ErrorPageProvider &errorPageProvider,
	const Logger &logger
) : ServiceBase(request, errorPageProvider, logger)
{
	struct stat statBuf;
	std::string filePath = request.getPath();

	if (filePath.empty() || filePath[0] != '/') {
		this->_response = this->_errorPageProvider.badRequest();
		LS_INFO() << "Invalid path: " << filePath << std::endl;
		return;
	}

	filePath = getRequestedFilePath(routeConfig, request.getNormalizedPath());

	if (stat(filePath.c_str(), &statBuf) != 0) {
		// TODO: errno見て適切に処理する
		this->_response = this->_errorPageProvider.notFound();
		LS_INFO() << "File not found: " << filePath << std::endl;
		return;
	}

	std::string lastModified = utils::getHttpTimeStr(statBuf.st_mtime);

	CS_LOG()
		<< "File info: "
		<< "User ID: " << statBuf.st_uid << ", "
		<< "Group ID: " << statBuf.st_gid << ", "
		<< "File size: " << statBuf.st_size << ", "
		<< "Block size: " << statBuf.st_blksize << ", "
		<< "Block count: " << statBuf.st_blocks << ", "
		<< "Permissions: " << utils::modeToString(statBuf.st_mode) << ", "
		<< "Last modified: " << lastModified << std::endl;

	if (!S_ISREG(statBuf.st_mode)) {
		this->_response = this->_errorPageProvider.permissionDenied();
		LS_INFO() << "Not a regular file: " << filePath << std::endl;
		return;
	}

	LS_DEBUG() << "Deleting file: " << filePath << std::endl;
	int deleteResult = std::remove(filePath.c_str());
	if (deleteResult != 0) {
		const errno_t errorNum = errno;
		CS_ERROR() << "Failed to read file (" << filePath << "): " << strerror(errorNum) << std::endl;
		this->_response = this->_errorPageProvider.internalServerError();
		return;
	}
	this->_response = this->_errorPageProvider.noContent();
	LS_INFO() << "File deleted: " << filePath << std::endl;
}

DeleteFileService::~DeleteFileService()
{
}

void DeleteFileService::setToPollFd(
	pollfd &pollFd
) const
{
	// Simpleの場合は、fdを使わないため、無視設定を行う。
	std::memset(
		&pollFd,
		0,
		sizeof(pollFd)
	);
}

ServiceEventResultType DeleteFileService::onEventGot(
	short revents
)
{
	(void)revents;
	return ServiceEventResult::COMPLETE;
}

}	 // namespace webserv

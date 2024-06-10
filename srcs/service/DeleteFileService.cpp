#include <sys/stat.h>

#include <Logger.hpp>
#include <cerrno>
#include <cstdio>
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
	const Logger &logger
) : ServiceBase(request, logger)
{
	const RequestedFileInfo &requestedFileInfo = request.getRequestedFileInfo();

	std::string filePath(requestedFileInfo.getTargetFilePath());

	// ファイルが存在すること等はRequestedFileInfoで確認済みのため、ここでは確認しない。

	LS_DEBUG() << "Deleting file: " << filePath << std::endl;
	int deleteResult = std::remove(filePath.c_str());
	if (deleteResult != 0) {
		const errno_t errorNum = errno;
		CS_ERROR() << "Failed to read file (" << filePath << "): " << std::strerror(errorNum) << std::endl;
		this->_response = this->getErrorPageProvider().internalServerError();
		return;
	}
	this->_response = this->getErrorPageProvider().noContent();
	LS_INFO() << "File deleted: " << filePath << std::endl;
}

DeleteFileService::~DeleteFileService()
{
}

void DeleteFileService::setToPollFd(
	pollfd &pollFd
) const
{
	pollFd.fd = -1;
	pollFd.events = 0;
	pollFd.revents = 0;
}

ServiceEventResultType DeleteFileService::onEventGot(
	short revents
)
{
	(void)revents;
	return ServiceEventResult::COMPLETE;
}

}	 // namespace webserv

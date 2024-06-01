#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cerrno>
#include <climits>
#include <cstring>
#include <iostream>
#include <macros.hpp>
#include <service/PostFileService.hpp>
#include <service/SimpleService.hpp>
#include <types.hpp>
#include <utils/getTimeStr.hpp>
#include <utils/modeToString.hpp>

namespace webserv
{

#define BUFFER_SIZE 4096

PostFileService::PostFileService(
	const HttpRequest &request,
	const RequestedFileInfo &requestedFileInfo,
	const webserv::utils::ErrorPageProvider &errorPageProvider,
	const Logger &logger
) : ServiceBase(request, errorPageProvider, logger),
		_fd(-1),
		_writtenSize(0)
{
	std::string filePath(requestedFileInfo.getTargetFilePath());

	// ファイルが存在しない場合も実行されるので注意
	if (requestedFileInfo.getIsNotFound()) {
		// TODO: 親ディレクトリが存在するか確認し、存在しなかったらNotFoundを返す
	}

	if (access(filePath.c_str(), W_OK) != 0) {
		this->_response = this->_errorPageProvider.permissionDenied();
		LS_INFO() << "Permission denied: " << filePath << std::endl;
		return;
	}

	this->_fd = open(filePath.c_str(), O_WRONLY | O_TRUNC);
	if (this->_fd < 0) {
		errno_t err = errno;
		this->_response = this->_errorPageProvider.internalServerError();
		LS_ERROR()
			<< "Failed to open file: " << filePath
			<< " (err: " << std::strerror(err) << ")"
			<< std::endl;
		return;
	}

	LS_DEBUG() << "Opened file: " << filePath << std::endl;
}

PostFileService::~PostFileService()
{
	if (0 <= this->_fd) {
		close(this->_fd);
		LS_DEBUG() << "Closed file descriptor: " << this->_fd << std::endl;
	}
}

void PostFileService::setToPollFd(
	pollfd &pollFd
) const
{
	if (this->_fd < 0) {
		pollFd.events = 0;
		return;
	}

	pollFd.fd = this->_fd;
	pollFd.events = POLLOUT;
}

ServiceEventResultType PostFileService::onEventGot(
	short revents
)
{
	if (this->_fd < 0) {
		return ServiceEventResult::COMPLETE;
	}

	if (!IS_POLLOUT(revents)) {
		return ServiceEventResult::CONTINUE;
	}

	size_t dataSize = this->_request.getBody().size() - this->_writtenSize;
	size_t writeSize = std::min(dataSize, static_cast<size_t>(INT_MAX));
	ssize_t writtenLength = write(
		this->_fd,
		this->_request.getBody().data() + this->_writtenSize,
		writeSize
	);

	LS_DEBUG() << "Write " << writtenLength << " bytes" << std::endl;
	if (writtenLength < 0) {
		const errno_t errorNum = errno;
		CS_ERROR() << "Failed to write: " << std::strerror(errorNum) << std::endl;
		this->_response = this->_errorPageProvider.internalServerError();

		return ServiceEventResult::COMPLETE;
	}

	this->_writtenSize += writtenLength;
	if (this->_request.getBody().size() == this->_writtenSize) {
		LS_DEBUG() << "Write " << writtenLength << " bytes" << std::endl;
		return ServiceEventResult::COMPLETE;
	}

	return ServiceEventResult::CONTINUE;
}

}	 // namespace webserv

#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <climits>
#include <cstring>
#include <iostream>
#include <macros.hpp>
#include <service/PostFileService.hpp>
#include <service/SimpleService.hpp>
#include <types.hpp>
#include <utils.hpp>

namespace webserv
{

#define BUFFER_SIZE 4096

PostFileService::PostFileService(
	const HttpRequest &request,
	const webserv::utils::ErrorPageProvider &errorPageProvider,
	const Logger &logger
) : ServiceBase(request, errorPageProvider, logger),
		_fd(-1)
{
	struct stat statBuf;
	std::string filePath = request.getPath();

	if (filePath.empty() || filePath[0] != '/') {
		this->_response = this->_errorPageProvider.badRequest();
		LS_INFO() << "Invalid path: " << filePath << std::endl;
		return;
	}

	filePath = "." + filePath;

	if (stat(filePath.c_str(), &statBuf) != 0) {
		// TODO: errno見て適切に処理する
		const errno_t errorNum = errno;
		if (errorNum != ENOENT) {
			this->_response = this->_errorPageProvider.internalServerError();
			LS_ERROR() << "Failed to open file: " << strerror(errorNum) << std::endl;
			return;
		}
		LS_INFO() << "File not found - creating file: " << filePath << std::endl;
		this->_fd = open(filePath.c_str(), O_CREAT | O_WRONLY, 0644);
		if (this->_fd < 0) {
			this->_response = this->_errorPageProvider.internalServerError();
			LS_ERROR() << "Failed to create file: " << filePath << std::endl;
			return;
		}
		this->_response.setStatusCode("201");
		this->_response.setReasonPhrase("Created");

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

	if (access(filePath.c_str(), W_OK) != 0) {
		this->_response = this->_errorPageProvider.permissionDenied();
		LS_INFO() << "Permission denied: " << filePath << std::endl;
		return;
	}

	this->_fd = open(filePath.c_str(), O_WRONLY | O_TRUNC);
	if (this->_fd < 0) {
		// TODO: errno見て適切に処理する
		this->_response = this->_errorPageProvider.internalServerError();
		LS_ERROR() << "Failed to open file: " << filePath << std::endl;
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
		CS_ERROR() << "Failed to write: " << strerror(errorNum) << std::endl;
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

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

static std::string getDirName(const std::string &path)
{
	size_t pos = path.rfind('/');
	if (pos == std::string::npos) {
		return "";
	}
	return path.substr(0, pos + 1);
}

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

	// ディレクトリにファイルを作成することはできない
	// AutoIndexファイルへの書き込みは許可しない
	// PathInfoがあるということは、目的でないファイルである可能性があるため、許可しない
	if (requestedFileInfo.getIsDirectory() || requestedFileInfo.getIsAutoIndexFile() || !requestedFileInfo.getCgiPathInfo().empty()) {
		this->_response = this->_errorPageProvider.methodNotAllowed();
		LS_INFO() << "Method not allowed: " << filePath << std::endl;
		return;
	}

	// ファイルが存在しなかった場合は、親ディレクトリが存在する場合にのみファイルの新規作成を行う
	if (requestedFileInfo.getIsNotFound()) {
		std::string dirPath = getDirName(filePath);
		struct stat statBuf;
		if (stat(dirPath.c_str(), &statBuf) != 0) {
			errno_t err = errno;
			this->_response = this->_errorPageProvider.notFound();
			LS_INFO()
				<< "Parent Directory Not Found: " << requestedFileInfo.getDocumentRoot()
				<< " (err: " << std::strerror(err) << ")"
				<< std::endl;
			return;
		}
		LS_LOG()
			<< "Directory info: "
			<< "Path: " << dirPath << ", "
			<< "User ID: " << statBuf.st_uid << ", "
			<< "Group ID: " << statBuf.st_gid << ", "
			<< "File size: " << statBuf.st_size << ", "
			<< "Block size: " << statBuf.st_blksize << ", "
			<< "Block count: " << statBuf.st_blocks << ", "
			<< "Permissions: " << utils::modeToString(statBuf.st_mode) << ", "
			<< "Last modified: " << utils::getHttpTimeStr(statBuf.st_mtime)
			<< std::endl;
		if (access(dirPath.c_str(), W_OK) != 0) {
			this->_response = this->_errorPageProvider.permissionDenied();
			LS_INFO() << "Permission denied: " << filePath << std::endl;
			return;
		}

		this->_fd = open(
			filePath.c_str(),
			O_WRONLY | O_CREAT | O_TRUNC,
			S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH
		);
	} else {
		if (access(filePath.c_str(), W_OK) != 0) {
			this->_response = this->_errorPageProvider.permissionDenied();
			LS_INFO() << "Permission denied: " << filePath << std::endl;
			return;
		}

		this->_fd = open(filePath.c_str(), O_WRONLY | O_TRUNC);
	}

	if (this->_fd < 0) {
		errno_t err = errno;
		if (err == EACCES) {
			this->_response = this->_errorPageProvider.permissionDenied();
		} else {
			this->_response = this->_errorPageProvider.internalServerError();
		}
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
	CS_LOG()
		<< "Writing " << writeSize << " / " << this->_request.getBody().size() << " bytes"
		<< " (already written: " << this->_writtenSize << ")"
		<< std::endl;
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
		LS_DEBUG() << "Write " << writtenLength << " bytes Completed" << std::endl;
		return ServiceEventResult::COMPLETE;
	}

	return ServiceEventResult::CONTINUE;
}

}	 // namespace webserv

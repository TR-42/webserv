#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstring>
#include <fstream>
#include <iostream>
#include <macros.hpp>
#include <service/GetFileService.hpp>
#include <service/SimpleService.hpp>

namespace webserv
{

#define BUFFER_SIZE 4096

GetFileService::GetFileService(
	const HttpRequest &request,
	const webserv::utils::ErrorPageProvider &errorPageProvider,
	const Logger &logger
) : ServiceBase(request, errorPageProvider, logger),
		_isDirectory(false),
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
		this->_response = this->_errorPageProvider.notFound();
		LS_INFO() << "File not found: " << filePath << std::endl;
		return;
	}

	if (!S_ISREG(statBuf.st_mode) && !S_ISDIR(statBuf.st_mode)) {
		this->_response = this->_errorPageProvider.permissionDenied();
		LS_INFO() << "Not a regular file or directory: " << filePath << std::endl;
		return;
	}

	this->_isDirectory = S_ISDIR(statBuf.st_mode);
	// ディレクトリの場合は、index.htmlを返す、ファイルの場合はパスを返す ルートから分ける？
	if (this->_isDirectory) {
		std::string indexFileName = "/index.html";
		std::string indexFilePath = filePath + indexFileName;
		if (stat(indexFilePath.c_str(), &statBuf) != 0) {
			this->_response = this->_errorPageProvider.notFound();
			LS_INFO() << "Index file not found: " << filePath << std::endl;
			return;
		}

		if (access(indexFilePath.c_str(), R_OK) != 0) {
			this->_response = this->_errorPageProvider.permissionDenied();
			LS_INFO() << "Permission denied: " << filePath << std::endl;
			return;
		}
	} else if (access(filePath.c_str(), R_OK) != 0) {
		this->_response = this->_errorPageProvider.permissionDenied();
		LS_INFO() << "Permission denied: " << filePath << std::endl;
		return;
	}

	this->_fd = open(filePath.c_str(), O_RDONLY);
	if (this->_fd < 0) {
		// TODO: errno見て適切に処理する
		this->_response = this->_errorPageProvider.internalServerError();
		LS_INFO() << "Failed to open file: " << filePath << std::endl;
		return;
	}

	LS_DEBUG() << "Opened file: " << filePath << std::endl;
}

GetFileService::~GetFileService()
{
}

void GetFileService::setToPollFd(
	pollfd &pollFd
) const
{
	if (this->_fd < 0) {
		pollFd.events = 0;
		return;
	}

	pollFd.fd = this->_fd;
	pollFd.events = POLLIN;
}

ServiceEventResultType GetFileService::onEventGot(
	short revents
)
{
	if (this->_fd < 0) {
		return ServiceEventResult::COMPLETE;
	}

	if (!IS_POLLIN(revents)) {
		return ServiceEventResult::CONTINUE;
	}

	uint8_t buf[BUFFER_SIZE];
	ssize_t readSize = read(this->_fd, buf, BUFFER_SIZE);
	if (readSize < 0) {
		// レスポンスにもエラーを設定する
		const errno_t errorNum = errno;
		CS_ERROR() << "Failed to read file: " << strerror(errorNum) << std::endl;
		this->_response = this->_errorPageProvider.internalServerError();

		return ServiceEventResult::COMPLETE;
	}

	if (readSize == 0) {
		std::string bodySize = std::to_string(this->_response.getBody().size());
		this->_response.getHeaders().addValue("Content-Length", bodySize);
		LS_INFO() << "File read complete: " << bodySize << " bytes" << std::endl;
		return ServiceEventResult::COMPLETE;
	}

	this->_response.getBody().insert(
		this->_response.getBody().end(),
		buf,
		buf + readSize
	);

	return ServiceEventResult::CONTINUE;
}

}	 // namespace webserv

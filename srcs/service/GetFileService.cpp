#include <sys/stat.h>
#include <unistd.h>

#include <cstring>
#include <fstream>
#include <iostream>
#include <service/GetFileService.hpp>
#include <service/SimpleService.hpp>

namespace webserv
{

GetFileService::GetFileService(
	const HttpRequest &request,
	const webserv::utils::ErrorPageProvider &errorPageProvider,
	const Logger &logger
) : _request(request),
		_errorPageProvider(errorPageProvider),
		_logger(logger)
{
	struct stat statBuf;
	std::string filePath = request.getPath();

	if (filePath.empty() || filePath[0] != '/') {
		this->_response = this->_errorPageProvider.badRequest();
		LS_INFO() << "Invalid path: " << filePath << std::endl;
		return;
	}

	if (stat(filePath.c_str(), &statBuf) != 0) {
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
}

GetFileService::~GetFileService()
{
}

void GetFileService::setToPollFd(
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

ServiceEventResultType GetFileService::onEventGot(
	short revents
)
{
	(void)revents;
	return ServiceEventResult::COMPLETE;
}

}	 // namespace webserv

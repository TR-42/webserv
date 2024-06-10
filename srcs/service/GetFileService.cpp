#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <Logger.hpp>
#include <algorithm>
#include <cerrno>
#include <config/HttpRouteConfig.hpp>
#include <cstring>
#include <iostream>
#include <macros.hpp>
#include <service/GetFileService.hpp>
#include <service/SimpleService.hpp>
#include <service/getRequestedFilePath.hpp>
#include <sstream>
#include <types.hpp>
#include <utils/getTimeStr.hpp>
#include <utils/modeToString.hpp>
#include <utils/normalizePath.hpp>
#include <utils/to_string.hpp>

namespace webserv
{

#define BUFFER_SIZE 4096

GetFileService::GetFileService(
	const HttpRequest &request,
	const webserv::utils::ErrorPageProvider &errorPageProvider,
	const Logger &logger
) : ServiceBase(request, errorPageProvider, logger),
		_isDirectory(request.getRequestedFileInfo().getIsDirectory()),
		_fd(-1)
{
	const RequestedFileInfo &requestedFileInfo = request.getRequestedFileInfo();

	std::string filePath(requestedFileInfo.getTargetFilePath());

	// ファイルが存在すること等はRequestedFileInfoで確認済みのため、ここでは確認しない。

	if (this->_isDirectory) {
		if (requestedFileInfo.getIsAutoIndexAllowed()) {
			this->generateFileList(filePath, request.getNormalizedPath());
		} else {
			this->_response = this->_errorPageProvider.notFound();
			LS_INFO() << "Document listing is disabled: " << filePath << std::endl;
		}
		return;
	}

	if (access(filePath.c_str(), R_OK) != 0) {
		this->_response = this->_errorPageProvider.permissionDenied();
		LS_INFO() << "Permission denied: " << filePath << std::endl;
		return;
	}

	this->_fd = open(filePath.c_str(), O_RDONLY | O_NONBLOCK | O_CLOEXEC);
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
	this->_response.getHeaders().addValue("Last-Modified", utils::getHttpTimeStr(requestedFileInfo.getStatBuf().st_mtime));
	static std::string _contentType = requestedFileInfo.getContentType();
	if (!_contentType.empty()) {
		this->_response.getHeaders().addValue("Content-Type", _contentType);
	}
}

GetFileService::~GetFileService()
{
	if (0 <= this->_fd) {
		close(this->_fd);
		LS_DEBUG() << "Closed file descriptor: " << this->_fd << std::endl;
	}
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
	LS_DEBUG() << "Read " << readSize << " bytes" << std::endl;
	if (readSize < 0) {
		const errno_t errorNum = errno;
		CS_ERROR() << "Failed to read file: " << std::strerror(errorNum) << std::endl;
		this->_response = this->_errorPageProvider.internalServerError();

		return ServiceEventResult::COMPLETE;
	}

	if (readSize == 0) {
		std::string bodySize = utils::to_string(this->_response.getBody().size());
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

void GetFileService::generateFileList(const std::string &filePath, const std::string &requestPath)
{
	DIR *dir;
	struct dirent *ent;
	std::vector<std::string> dirVector;
	std::vector<std::string> fileVector;
	std::string parentDirLint = "";
	if (filePath == "./") {
		parentDirLint = "";
	} else {
		parentDirLint = "<li><a href=\"../\">../</a></li>";
	}

	if ((dir = opendir(filePath.c_str())) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			if (ent->d_name[0] == '.') {
				continue;
			}
			if (ent->d_type == DT_DIR) {
				dirVector.push_back(ent->d_name);
			} else if (ent->d_type == DT_REG) {
				fileVector.push_back(ent->d_name);
			}
		}
		closedir(dir);
	} else {
		errno_t err = errno;
		this->_response = this->_errorPageProvider.internalServerError();
		LS_ERROR()
			<< "Failed to open directory: " << filePath
			<< " (err: " << std::strerror(err) << ")"
			<< std::endl;
		return;
	}

	std::sort(dirVector.begin(), dirVector.end());
	std::sort(fileVector.begin(), fileVector.end());

	std::stringstream html;
	html << "<!DOCTYPE html><html><head><title>Index of " << requestPath << "</title></head>";
	html << "<body>";
	html << "<h1>Index of " << requestPath << "</h1>";
	html << "<ul>";
	html << parentDirLint;
	for (std::vector<std::string>::const_iterator it = dirVector.begin(); it != dirVector.end(); ++it) {
		html
			<< "<li><a href=\""
			<< utils::normalizePath("/" + requestPath + "/" + *it)
			<< "/\">"
			<< *it
			<< "/</a></li>";
	}
	for (std::vector<std::string>::const_iterator it = fileVector.begin(); it != fileVector.end(); ++it) {
		html
			<< "<li><a href=\""
			<< utils::normalizePath("/" + requestPath + "/" + *it)
			<< "\">"
			<< *it
			<< "</a></li>";
	}

	std::string fileList = html.str();
	this->_response.getBody().insert(
		this->_response.getBody().end(),
		fileList.begin(),
		fileList.end()
	);
	this->_response.getHeaders().addValue("Content-Type", "text/html");
}

}	 // namespace webserv

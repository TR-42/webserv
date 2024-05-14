#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <Logger.hpp>
#include <algorithm>
#include <config/HttpRouteConfig.hpp>
#include <cstring>
#include <iostream>
#include <macros.hpp>
#include <service/GetFileService.hpp>
#include <service/SimpleService.hpp>
#include <service/getRequestedFilePath.hpp>
#include <sstream>
#include <types.hpp>
#include <utils.hpp>

namespace webserv
{

#define BUFFER_SIZE 4096

GetFileService::GetFileService(
	const HttpRequest &request,
	const HttpRouteConfig &routeConfig,
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

	if (!S_ISREG(statBuf.st_mode) && !S_ISDIR(statBuf.st_mode)) {
		this->_response = this->_errorPageProvider.permissionDenied();
		LS_INFO() << "Not a regular file or directory: " << filePath << std::endl;
		return;
	}

	this->_isDirectory = S_ISDIR(statBuf.st_mode);
	if (this->_isDirectory) {
		std::string indexFileName = "/index.html";
		std::string indexFilePath = filePath + indexFileName;
		if (stat(indexFilePath.c_str(), &statBuf) != 0 || !S_ISREG(statBuf.st_mode)) {
			if (!routeConfig.getIsDocumentListingEnabled()) {
				this->_response = this->_errorPageProvider.notFound();
				LS_INFO()
					<< "Index file not found: " << filePath
					<< " Document listing is disabled."
					<< std::endl;
				return;
			}

			this->generateFileList(filePath, request.getNormalizedPath());
			LS_INFO()
				<< "Index file not found: " << filePath
				<< "\tS_ISREG: " << std::boolalpha << S_ISREG(statBuf.st_mode)
				<< std::endl;
			return;
		}

		if (!S_ISREG(statBuf.st_mode)) {
			this->_response = this->_errorPageProvider.permissionDenied();
			LS_INFO() << "Not a regular file: " << filePath << std::endl;
			return;
		}

		if (access(indexFilePath.c_str(), R_OK) != 0) {
			this->_response = this->_errorPageProvider.permissionDenied();
			LS_INFO() << "Permission denied: " << filePath << std::endl;
			return;
		}

		filePath = indexFilePath;
	} else if (access(filePath.c_str(), R_OK) != 0) {
		this->_response = this->_errorPageProvider.permissionDenied();
		LS_INFO() << "Permission denied: " << filePath << std::endl;
		return;
	}

	this->_fd = open(filePath.c_str(), O_RDONLY);
	if (this->_fd < 0) {
		// TODO: errno見て適切に処理する
		this->_response = this->_errorPageProvider.internalServerError();
		LS_ERROR() << "Failed to open file: " << filePath << std::endl;
		return;
	}

	LS_DEBUG() << "Opened file: " << filePath << std::endl;
	this->_response.getHeaders().addValue("Last-Modified", lastModified);
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
		this->_canDispose = true;
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
		CS_ERROR() << "Failed to read file: " << strerror(errorNum) << std::endl;
		this->_response = this->_errorPageProvider.internalServerError();

		this->_canDispose = true;
		return ServiceEventResult::COMPLETE;
	}

	if (readSize == 0) {
		std::string bodySize = utils::to_string(this->_response.getBody().size());
		this->_response.getHeaders().addValue("Content-Length", bodySize);
		LS_INFO() << "File read complete: " << bodySize << " bytes" << std::endl;
		this->_canDispose = true;
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
		this->_response = this->_errorPageProvider.internalServerError();
		LS_LOG() << "Failed to open directory: " << filePath << std::endl;
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

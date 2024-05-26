#include <sys/stat.h>

#include <Logger.hpp>
#include <cerrno>
#include <config/HttpRouteConfig.hpp>
#include <cstdio>
#include <iostream>
#include <service/RequestedFileInfo.hpp>
#include <utils/getTimeStr.hpp>
#include <utils/modeToString.hpp>

namespace webserv
{

static inline std::string joinPath(
	const HttpRouteConfig &routeConfig,
	const std::vector<std::string> &pathSegList
)
{
	std::string path = routeConfig.getDocumentRoot();

	for (
		std::vector<std::string>::const_iterator it = pathSegList.begin() + routeConfig.getRequestPathSegmentList().size();
		it != pathSegList.end();
		++it
	) {
		path += PATH_SEPARATOR;
		path += *it;
	}

	return path;
}

static inline std::string joinPath(
	const std::string &documentRoot,
	const std::string &pathSegList
)
{
	return documentRoot + PATH_SEPARATOR + pathSegList;
}

RequestedFileInfo::RequestedFileInfo(
	const std::vector<std::string> &requestedPathSegList,
	const bool isRequestEndWithSlash,
	const HttpRouteConfig &routeConfig,
	const Logger &logger
) : _TargetFilePath(joinPath(routeConfig, requestedPathSegList)),
		_DocumentRoot(routeConfig.getDocumentRoot()),
		_IsDirectory(isRequestEndWithSlash),
		_IsNotFound(false),
		_IsCgi(false),
		_CgiConfig(),
		_StatBuf(),
		_FileExtensionWithoutDot()
{
	L_LOG("Process Start");

	if (this->_checkTargetFilePathStat(isRequestEndWithSlash, logger)) {
		return;
	}

	// (通常ファイルへのリクエストの場合は処理されない)
	this->_findIndexFile(routeConfig, logger);

	// (ディレクトリへのリクエストの場合は処理されない)
	this->_pickFileExtensionWithoutDot(logger);
	this->_pickCgiConfig(routeConfig, logger);

	L_LOG("Process End");
}

bool RequestedFileInfo::_checkTargetFilePathStat(
	const bool isRequestEndWithSlash,
	const Logger &logger
)
{
	struct stat statBuf;

	if (stat(this->_TargetFilePath.c_str(), &statBuf) != 0) {
		errno_t err = errno;
		_IsNotFound = true;
		LS_INFO()
			<< "File (or directory) not found: " << _TargetFilePath
			<< " (err: " << std::strerror(err) << ")"
			<< std::endl;
		return false;
	}

	this->_StatBuf = statBuf;
	if (isRequestEndWithSlash && S_ISREG(statBuf.st_mode)) {
		_IsNotFound = true;
		LS_INFO()
			<< "Not a directory (but regular file): " << _TargetFilePath
			<< std::endl;
		return false;
	}

	this->_IsDirectory = S_ISDIR(statBuf.st_mode);
	if (!this->_IsDirectory && !S_ISREG(statBuf.st_mode)) {
		_IsNotFound = true;
		LS_INFO()
			<< "Not a regular file or directory: " << _TargetFilePath
			<< std::endl;
		return false;
	}

	LS_LOG()
		<< "File/Directory info: "
		<< "IsDirectory: " << std::boolalpha << this->_IsDirectory << ", "
		<< "User ID: " << statBuf.st_uid << ", "
		<< "Group ID: " << statBuf.st_gid << ", "
		<< "File size: " << statBuf.st_size << ", "
		<< "Block size: " << statBuf.st_blksize << ", "
		<< "Block count: " << statBuf.st_blocks << ", "
		<< "Permissions: " << utils::modeToString(statBuf.st_mode) << ", "
		<< "Last modified: " << utils::getHttpTimeStr(statBuf.st_mtime) << std::endl;

	return true;
}

void RequestedFileInfo::_findIndexFile(
	const HttpRouteConfig &routeConfig,
	const Logger &logger
)
{
	if (!this->_IsDirectory) {
		L_LOG("Not a directory -> Skip");
		return;
	}

	LS_DEBUG()
		<< "Finding index file (index file list length: " << routeConfig.getIndexFileList().size() << ")"
		<< std::endl;

	std::vector<std::string>::const_iterator itEnd = routeConfig.getIndexFileList().end();
	for (
		std::vector<std::string>::const_iterator it = routeConfig.getIndexFileList().begin();
		it != itEnd;
		++it
	) {
		std::string indexFilePath = joinPath(this->_TargetFilePath, *it);
		struct stat statBuf;

		if (stat(indexFilePath.c_str(), &statBuf) != 0) {
			errno_t err = errno;
			LS_DEBUG()
				<< "stat error for path: " << indexFilePath
				<< " (err: " << std::strerror(err) << ")"
				<< std::endl;
			continue;
		}

		if (!S_ISREG(statBuf.st_mode)) {
			LS_LOG()
				<< "Not a regular file: " << indexFilePath
				<< std::endl;
			continue;
		}

		this->_TargetFilePath = indexFilePath;
		this->_IsDirectory = false;
		this->_StatBuf = statBuf;
		LS_INFO()
			<< "Index file found: " << indexFilePath
			<< std::endl;

		std::string lastModified = utils::getHttpTimeStr(statBuf.st_mtime);

		LS_LOG()
			<< "IndexFile info: "
			<< "User ID: " << statBuf.st_uid << ", "
			<< "Group ID: " << statBuf.st_gid << ", "
			<< "File size: " << statBuf.st_size << ", "
			<< "Block size: " << statBuf.st_blksize << ", "
			<< "Block count: " << statBuf.st_blocks << ", "
			<< "Permissions: " << utils::modeToString(statBuf.st_mode) << ", "
			<< "Last modified: " << lastModified << std::endl;
		return;
	}

	L_DEBUG("Index file not found");
}

void RequestedFileInfo::_pickFileExtensionWithoutDot(
	const Logger &logger
)
{
	if (this->_IsDirectory) {
		return;
	}

	std::string::size_type dotPos = this->_TargetFilePath.rfind('.');
	if (dotPos == std::string::npos) {
		LS_DEBUG()
			<< "No file extension: " << this->_TargetFilePath << ""
			<< std::endl;
		return;
	}

	this->_FileExtensionWithoutDot = this->_TargetFilePath.substr(dotPos + 1);

	LS_DEBUG()
		<< "File extension without dot: `" << this->_FileExtensionWithoutDot << "`"
		<< std::endl;
}

void RequestedFileInfo::_pickCgiConfig(
	const HttpRouteConfig &routeConfig,
	const Logger &logger
)
{
	if (this->_IsDirectory) {
		return;
	}

	std::vector<CgiConfig>::const_iterator itEnd = routeConfig.getCgiConfigList().end();
	for (
		std::vector<CgiConfig>::const_iterator it = routeConfig.getCgiConfigList().begin();
		it != itEnd;
		++it
	) {
		if (it->getExtensionWithoutDot() != this->_FileExtensionWithoutDot) {
			continue;
		}

		this->_IsCgi = true;
		this->_CgiConfig = *it;
		LS_INFO()
			<< "CGI file found: " << this->_TargetFilePath
			<< " (CGI executable: " << this->_CgiConfig.getCgiExecutableFullPath() << ")"
			<< std::endl;
		break;
	}
}

RequestedFileInfo::RequestedFileInfo(
	const RequestedFileInfo &other
) : _TargetFilePath(other._TargetFilePath),
		_DocumentRoot(other._DocumentRoot),
		_IsDirectory(other._IsDirectory),
		_IsNotFound(other._IsNotFound),
		_IsCgi(other._IsCgi),
		_CgiConfig(other._CgiConfig),
		_StatBuf(other._StatBuf),
		_FileExtensionWithoutDot(other._FileExtensionWithoutDot)
{
}

RequestedFileInfo &webserv::RequestedFileInfo::operator=(const RequestedFileInfo &other)
{
	if (this == &other) {
		return *this;
	}

	this->_TargetFilePath = other._TargetFilePath;
	this->_DocumentRoot = other._DocumentRoot;
	this->_IsDirectory = other._IsDirectory;
	this->_IsNotFound = other._IsNotFound;
	this->_IsCgi = other._IsCgi;
	this->_CgiConfig = other._CgiConfig;
	this->_StatBuf = other._StatBuf;
	this->_FileExtensionWithoutDot = other._FileExtensionWithoutDot;

	return *this;
}

RequestedFileInfo::~RequestedFileInfo()
{
}

}	 // namespace webserv

#include <sys/stat.h>

#include <Logger.hpp>
#include <algorithm>
#include <cerrno>
#include <config/HttpRouteConfig.hpp>
#include <cstring>
#include <iostream>
#include <service/RequestedFileInfo.hpp>
#include <stdexcept>
#include <utils/getTimeStr.hpp>
#include <utils/modeToString.hpp>

namespace webserv
{

RequestedFileInfo::RequestedFileInfo(
	const std::vector<std::string> &requestedPathSegList,
	const bool isRequestEndWithSlash,
	const HttpRouteConfig &routeConfig,
	const Logger &logger
) : _CgiScriptName(joinPath(requestedPathSegList)),
		_CgiPathInfo(),
		_TargetFilePathWithoutDocumentRoot(pickTargetFilePathWithoutDocumentRoot(routeConfig, requestedPathSegList)),
		_DocumentRoot(routeConfig.getDocumentRoot()),
		_IsDirectory(isRequestEndWithSlash),
		_IsNotFound(false),
		_IsCgi(false),
		_IsAutoIndexAllowed(routeConfig.getIsDocumentListingEnabled()),
		_IsAutoIndexFile(false),
		_CgiConfig(),
		_StatBuf(),
		_FileExtensionWithoutDot()
{
	L_LOG("Process Start");

	size_t reqPathSegListSize = requestedPathSegList.size();
	size_t configReqPathSegListSize = routeConfig.getRequestPathSegmentList().size();
	if (reqPathSegListSize < configReqPathSegListSize) {
		L_ERROR("Invalid Argument (path length error)");
		this->_IsNotFound = true;
		return;
	}

	if (!this->_checkTargetFilePathStat(isRequestEndWithSlash, requestedPathSegList, configReqPathSegListSize, logger)) {
		return;
	}

	// (通常ファイルへのリクエストの場合は処理されない)
	this->_findIndexFile(routeConfig, logger);

	if (this->_IsNotFound) {
		L_LOG("Process End");
		return;
	}

	// (ディレクトリへのリクエストの場合は処理されない)
	this->_pickFileExtensionWithoutDot(logger);
	this->_pickCgiConfig(routeConfig, logger);

	// ファイルに対するリクエストの場合にのみCgiPathInfoが設定されている
	bool isCgiPathInfoAvailable = !this->_CgiPathInfo.empty();
	this->_IsNotFound = !this->_IsCgi && isCgiPathInfoAvailable;

	this->_ContentType = this->_getContentType(routeConfig);

	L_LOG("Process End");
}

bool RequestedFileInfo::_checkTargetFilePathStat(
	const bool isRequestEndWithSlash,
	const std::vector<std::string> requestedPathSegList,
	size_t configReqPathSegListSize,
	const Logger &logger
)
{
	if (requestedPathSegList.size() == configReqPathSegListSize) {
		L_DEBUG("No path segments");

		struct stat statBuf;
		if (stat(this->_DocumentRoot.c_str(), &statBuf) != 0) {
			errno_t err = errno;
			this->_IsNotFound = true;
			LS_INFO()
				<< "Document root not found: " << this->_DocumentRoot
				<< " (err: " << std::strerror(err) << ")"
				<< std::endl;
			return false;
		}

		if (!S_ISDIR(statBuf.st_mode)) {
			this->_IsNotFound = true;
			LS_INFO()
				<< "Document root is not a directory: " << this->_DocumentRoot
				<< std::endl;
			return false;
		}

		this->_IsDirectory = true;
		this->_StatBuf = statBuf;

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

		return true;
	}

	std::string path(this->_DocumentRoot);
	struct stat statBuf;
	for (size_t i = configReqPathSegListSize; i < requestedPathSegList.size(); ++i) {
		path = joinPath(path, requestedPathSegList[i]);
		if (stat(path.c_str(), &statBuf) != 0) {
			errno_t err = errno;
			this->_IsNotFound = true;
			LS_INFO()
				<< "File (or directory) not found: " << path
				<< " (err: " << std::strerror(err) << ")"
				<< std::endl;
			return false;
		}

		if (!S_ISDIR(statBuf.st_mode) && !S_ISREG(statBuf.st_mode)) {
			this->_IsNotFound = true;
			LS_INFO()
				<< "not a regular file or directory: " << path
				<< std::endl;
			return false;
		}

		if (S_ISDIR(statBuf.st_mode)) {
			this->_IsDirectory = true;
			this->_StatBuf = statBuf;
			LS_LOG()
				<< "Directory found: " << path
				<< std::endl;
		} else {
			this->_IsDirectory = false;
			this->_StatBuf = statBuf;
			this->_FileName = requestedPathSegList[i];
			this->_TargetFilePathWithoutDocumentRoot = joinPath(requestedPathSegList, configReqPathSegListSize, i - configReqPathSegListSize + 1);
			this->_CgiScriptName = joinPath(requestedPathSegList, 0, i + 1);
			this->_CgiPathInfo = joinPath(requestedPathSegList, i + 1);
			if (isRequestEndWithSlash) {
				this->_CgiPathInfo += PATH_SEPARATOR;
			}
			LS_LOG()
				<< "File found: " << path
				<< ", FileName: " << this->_FileName
				<< ", TargetFilePathWithoutDocumentRoot: " << this->_TargetFilePathWithoutDocumentRoot
				<< ", CgiScriptName: " << this->_CgiScriptName
				<< ", CgiPathInfo: " << this->_CgiPathInfo
				<< std::endl;
			LS_LOG()
				<< "File info: "
				<< "User ID: " << statBuf.st_uid << ", "
				<< "Group ID: " << statBuf.st_gid << ", "
				<< "File size: " << statBuf.st_size << ", "
				<< "Block size: " << statBuf.st_blksize << ", "
				<< "Block count: " << statBuf.st_blocks << ", "
				<< "Permissions: " << utils::modeToString(statBuf.st_mode) << ", "
				<< "Last modified: " << utils::getHttpTimeStr(statBuf.st_mtime) << std::endl;
			return true;
		}
	}

	LS_LOG()
		<< "It was directory: " << path
		<< std::endl;

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

	std::string targetFilePathWithDocRoot = joinPath(this->_DocumentRoot, this->_TargetFilePathWithoutDocumentRoot);

	std::vector<std::string>::const_iterator itEnd = routeConfig.getIndexFileList().end();
	for (
		std::vector<std::string>::const_iterator it = routeConfig.getIndexFileList().begin();
		it != itEnd;
		++it
	) {
		std::string indexFilePath = joinPath(targetFilePathWithDocRoot, *it);
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

		this->_FileName = *it;
		this->_TargetFilePathWithoutDocumentRoot = joinPath(this->_TargetFilePathWithoutDocumentRoot, *it);
		this->_CgiScriptName = joinPath(this->_CgiScriptName, *it);
		this->_IsDirectory = false;
		this->_IsAutoIndexFile = true;
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
	this->_IsNotFound = !this->_IsAutoIndexAllowed;
}

void RequestedFileInfo::_pickFileExtensionWithoutDot(
	const Logger &logger
)
{
	if (this->_IsDirectory) {
		L_LOG("Directory -> Skip");
		return;
	}

	// 正規化されているため、ドットが存在しない場合は拡張子が存在しないということになる
	std::string::size_type dotPos = this->_FileName.rfind('.');
	if (dotPos == std::string::npos || dotPos == 0) {
		LS_DEBUG()
			<< "No file extension: " << this->_FileName << ""
			<< std::endl;
		return;
	}

	std::string fileExtensionWithoutDot = this->_FileName.substr(dotPos + 1);
	std::transform(fileExtensionWithoutDot.begin(), fileExtensionWithoutDot.end(), fileExtensionWithoutDot.begin(), ::tolower);
	this->_FileExtensionWithoutDot = fileExtensionWithoutDot;

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
		L_LOG("Directory -> Skip");
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
			<< "CGI file found: " << this->_TargetFilePathWithoutDocumentRoot
			<< " (CGI executable: " << this->_CgiConfig.getCgiExecutableFullPath() << ")"
			<< std::endl;
		return;
	}

	LS_DEBUG()
		<< "CGI file not found: " << this->_TargetFilePathWithoutDocumentRoot
		<< std::endl;
}

RequestedFileInfo::RequestedFileInfo(
	const RequestedFileInfo &other
) : _TargetFilePathWithoutDocumentRoot(other._TargetFilePathWithoutDocumentRoot),
		_DocumentRoot(other._DocumentRoot),
		_IsDirectory(other._IsDirectory),
		_IsNotFound(other._IsNotFound),
		_IsCgi(other._IsCgi),
		_IsAutoIndexAllowed(other._IsAutoIndexAllowed),
		_CgiConfig(other._CgiConfig),
		_StatBuf(other._StatBuf),
		_FileExtensionWithoutDot(other._FileExtensionWithoutDot),
		_ContentType(other._ContentType)
{
}

RequestedFileInfo &webserv::RequestedFileInfo::operator=(const RequestedFileInfo &other)
{
	if (this == &other) {
		return *this;
	}

	this->_TargetFilePathWithoutDocumentRoot = other._TargetFilePathWithoutDocumentRoot;
	this->_DocumentRoot = other._DocumentRoot;
	this->_IsDirectory = other._IsDirectory;
	this->_IsNotFound = other._IsNotFound;
	this->_IsCgi = other._IsCgi;
	this->_IsAutoIndexAllowed = other._IsAutoIndexAllowed;
	this->_CgiConfig = other._CgiConfig;
	this->_StatBuf = other._StatBuf;
	this->_FileExtensionWithoutDot = other._FileExtensionWithoutDot;
	this->_ContentType = other._ContentType;

	return *this;
}

RequestedFileInfo::~RequestedFileInfo()
{
}

}	 // namespace webserv

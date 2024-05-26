#pragma once

#include <sys/stat.h>

#include <Logger.hpp>
#include <classDefUtils.hpp>
#include <config/HttpRouteConfig.hpp>

#define PATH_SEPARATOR '/'

namespace webserv
{

class RequestedFileInfo
{
 private:
	DECL_VAR_REF_GETTER(std::string, TargetFilePath);
	DECL_VAR_REF_GETTER(std::string, DocumentRoot);
	DECL_VAR_GETTER(bool, IsDirectory);
	DECL_VAR_GETTER(bool, IsNotFound);
	DECL_VAR_GETTER(bool, IsCgi);
	DECL_VAR_REF_GETTER(CgiConfig, CgiConfig);
	DECL_VAR_REF_GETTER(struct stat, StatBuf);
	DECL_VAR_REF_GETTER(std::string, FileExtensionWithoutDot);

	bool _checkTargetFilePathStat(
		const bool isRequestEndWithSlash,
		const Logger &logger
	);
	void _findIndexFile(
		const HttpRouteConfig &routeConfig,
		const Logger &logger
	);
	void _pickFileExtensionWithoutDot(
		const Logger &logger
	);
	void _pickCgiConfig(
		const HttpRouteConfig &routeConfig,
		const Logger &logger
	);

 public:
	RequestedFileInfo(
		const std::vector<std::string> &requestedPathSegList,
		const bool isRequestEndWithSlash,
		const HttpRouteConfig &routeConfig,
		const Logger &logger
	);
	RequestedFileInfo(const RequestedFileInfo &other);
	RequestedFileInfo &operator=(const RequestedFileInfo &other);
	virtual ~RequestedFileInfo();
};

}	 // namespace webserv

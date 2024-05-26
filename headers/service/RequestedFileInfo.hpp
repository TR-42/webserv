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
	DECL_VAR_REF_GETTER(std::string, CgiScriptName);
	DECL_VAR_REF_GETTER(std::string, CgiPathInfo);	// TODO: 実装
	DECL_VAR_REF_GETTER(std::string, TargetFilePathWithoutDocumentRoot);
	DECL_VAR_REF_GETTER(std::string, DocumentRoot);
	DECL_VAR_GETTER(bool, IsDirectory);
	DECL_VAR_GETTER(bool, IsNotFound);
	DECL_VAR_GETTER(bool, IsCgi);
	DECL_VAR_GETTER(bool, IsAutoIndexAllowed);
	DECL_VAR_REF_GETTER(CgiConfig, CgiConfig);
	DECL_VAR_REF_GETTER(struct stat, StatBuf);
	DECL_VAR_REF_GETTER(std::string, FileExtensionWithoutDot);

	// TODO: Path Infoの実装

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

	inline std::string getTargetFilePath() const
	{
		return RequestedFileInfo::joinPath(
			this->getDocumentRoot(),
			this->getTargetFilePathWithoutDocumentRoot()
		);
	}

	static inline std::string joinPath(
		const std::string &documentRoot,
		const std::string &pathSeg
	)
	{
		return documentRoot + PATH_SEPARATOR + pathSeg;
	}

	static inline std::string joinPath(
		const std::vector<std::string> &pathSegList
	)
	{
		std::string path;

		for (
			std::vector<std::string>::const_iterator it = pathSegList.begin();
			it != pathSegList.end();
			++it
		) {
			path += PATH_SEPARATOR;
			path += *it;
		}

		return path;
	}

	static inline std::string pickTargetFilePathWithoutDocumentRoot(
		const HttpRouteConfig &routeConfig,
		const std::vector<std::string> &pathSegList
	)
	{
		std::string path;

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
};

}	 // namespace webserv

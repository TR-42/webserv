#pragma once

#include <sys/stat.h>

#include <Logger.hpp>
#include <classDefUtils.hpp>
#include <config/HttpRouteConfig.hpp>
#include <limits>
#include <math/math.hpp>

#define PATH_SEPARATOR '/'

#ifdef SIZE_MAX
#undef SIZE_MAX
#endif

namespace webserv
{

const size_t SIZE_MAX = std::numeric_limits<size_t>::max();
class RequestedFileInfo
{
 private:
	DECL_VAR_REF_GETTER(std::string, FileName);
	DECL_VAR_REF_GETTER(std::string, CgiScriptName);
	DECL_VAR_REF_GETTER(std::string, CgiPathInfo);
	DECL_VAR_REF_GETTER(std::string, TargetFilePathWithoutDocumentRoot);
	DECL_VAR_REF_GETTER(std::string, DocumentRoot);
	DECL_VAR_GETTER(bool, IsDirectory);
	DECL_VAR_GETTER(bool, IsNotFound);
	DECL_VAR_GETTER(bool, IsCgi);
	DECL_VAR_GETTER(bool, IsAutoIndexAllowed);
	DECL_VAR_REF_GETTER(CgiConfig, CgiConfig);
	DECL_VAR_REF_GETTER(struct stat, StatBuf);
	DECL_VAR_REF_GETTER(std::string, FileExtensionWithoutDot);

	bool _checkTargetFilePathStat(
		const bool isRequestEndWithSlash,
		const std::vector<std::string> requestedPathSegList,
		size_t configReqPathSegListSize,
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

	inline std::string getCgiPathTranslated() const
	{
		if (this->_CgiPathInfo.empty()) {
			return "";
		}

		return this->getDocumentRoot() + this->getCgiPathInfo();
	}

	static inline std::string joinPath(
		const std::string &documentRoot,
		const std::string &pathSeg
	)
	{
		if (documentRoot.empty()) {
			return pathSeg;
		} else if (pathSeg.empty()) {
			return documentRoot;
		}

		bool isDocumentRootEndWithSlash = documentRoot[documentRoot.length() - 1] == PATH_SEPARATOR;
		bool isPathSegStartWithSlash = pathSeg[0] == PATH_SEPARATOR;
		if (isDocumentRootEndWithSlash && isPathSegStartWithSlash) {
			return documentRoot + pathSeg.substr(1);
		} else if (isDocumentRootEndWithSlash || isPathSegStartWithSlash) {
			return documentRoot + pathSeg;
		} else {
			return documentRoot + PATH_SEPARATOR + pathSeg;
		}
	}

	static inline std::string joinPath(
		const std::vector<std::string> &pathSegList,
		size_t start = 0,
		size_t maxLength = SIZE_MAX
	)
	{
		std::string path;

		if (pathSegList.size() <= start) {
			return path;
		}

		if (math::can_add(start, maxLength)) {
			maxLength += start;
		}

		for (
			size_t i = start;
			i < pathSegList.size() && i < maxLength;
			++i
		) {
			path += PATH_SEPARATOR;
			path += pathSegList[i];
		}

		return path;
	}

	static inline std::string pickTargetFilePathWithoutDocumentRoot(
		const HttpRouteConfig &routeConfig,
		const std::vector<std::string> &pathSegList
	)
	{
		std::string path;

		size_t configPathSegListSize = routeConfig.getRequestPathSegmentList().size();
		size_t pathSegListSize = pathSegList.size();
		if (pathSegListSize <= configPathSegListSize) {
			return path;
		}

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
